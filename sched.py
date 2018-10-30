import datetime,time,sys,pytz,os,socket,signal,shlex
import subprocess as sp
from apscheduler.schedulers.blocking import BlockingScheduler
from utils import run_cmd_shell_wtimer,run_cmd_wtimer,run_cmd_shell

def tshark(dir,filename):
    cmd = 'cd %s;f=%s;tshark -r $f -Tfields -o tcp.relative_sequence_numbers:FALSE -e ip.id -e tcp.srcport -e tcp.dstport -e tcp.seq -e tcp.ack -e tcp.options.timestamp.tsecr -e tcp.options.timestamp.tsval > ${f%%.*}_1.txt;sort ${f%%.*}_1.txt > ${f%%.*}.tshark;rm ${f%%.*}_1.txt' % (dir,filename)
    run_cmd_shell(cmd)


def tshark_capture(out_dir,interface,remote_ip,remote_hostname,role):
    global seq
    print('tshark: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
    out_filename = 'loss_%s_%s_%s_%d_%s.tshark' % (socket.gethostname(),role,remote_hostname,seq,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
    tshark_cmd = 'sudo tshark -i %s -f \'host %s and tcp port %s\' -Tfields -o tcp.relative_sequence_numbers:FALSE -e ip.id -e tcp.srcport -e tcp.dstport -e tcp.seq -e tcp.ack -e tcp.options.timestamp.tsecr -e tcp.options.timestamp.tsval > %s'
    run_cmd_shell_wtimer(tshark_cmd % (interface,remote_ip,80,os.path.join(out_dir,out_filename)),10)
    seq += 1
    print('tshark: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')+'\n')

intvls = [10,5,1,0.1,0.01]

def tcpdump_tshark(out_dir,interface,remote_ip,remote_hostname,role,size):
    global seq, sess_intvl
    print('tcpdump_tshark: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
    out_filename = 'loss_%s_%s_%s_%02d_%.2f_%d_%s.pcap' % (socket.gethostname(),role,remote_hostname,seq,intvls[seq],size,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
    run_cmd_wtimer('tcpdump -w %s -i %s -n host %s and tcp port 80' % (os.path.join(out_dir,out_filename),interface,remote_ip),5+sess_intvl+20)
    seq += 1
    #tshark(out_dir,out_filename)    
    print('tcpdump_tshark: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')+'\n')


def tcpdump_icmp(out_dir,interface,remote_ip,remote_hostname):
    global seq
    print('tcpdump_icmp: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
    out_filename = 'loss_%s_%s_%s_%02d_%s_size.pcap' % (socket.gethostname(),remote_hostname,seq,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
    run_cmd_wtimer('tcpdump -w %s -i %s -n host %s and icmp' % (os.path.join(out_dir,out_filename),interface,remote_ip),615)
    seq += 1
    #tshark(out_dir,out_filename)    
    print('tcpdump_icmp: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')+'\n')




def loss_test(out_dir,interface,remote_ip,remote_hostname,role,seq):
    out_filename = 'loss_%s_%s_%s_%d_%s.pcap' % (socket.gethostname(),role,remote_hostname,seq,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
    tshark_cmd = 'tshark -i %s -f \'host %s  and tcp port %s\' -Tfields -o tcp.relative_sequence_numbers:FALSE -e ip.id -e tcp.srcport -e tcp.dstport -e tcp.seq -e tcp.ack -e tcp.options.timestamp.tsecr -e tcp.options.timestamp.tsval > %s'
    p_td = sp.Popen(tshark_cmd % (os.path.join(out_dir,out_filename),interface,remote_ip),shell=True, preexec_fn=os.setpgrp)
    #do something

    os.killpg(os.getpgid(p_td.pid), signal.SIGTERM)

# def client_curl():
#     global start_str
#     print('client_curl: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
#     run_cmd_shell_wtimer('script -aqf -c \'while true;do curl -o /dev/null --limit-rate 500k --speed-time 120 -LJv4k http://169.235.31.181:20000/my.pcap; sleep 10;done\' %s/curl_%s_terran_loss_test_%s.txt'% (out_dir,socket.gethostname(),start_str),7200)
#     print('client_curl: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')+'\n')

def client_sender():
    sp.call(shlex.split('%s/sanity_test/sender_client 169.235.31.181' % os.path.expanduser('~')))

def server_sender(size,sess_intvl):
    sp.call(shlex.split('%s/sanity_test/sender_server %d %d 0' % (os.path.expanduser('~'),size,sess_intvl)))

def sep_sender(intf,rem_ip,rem_hn,role):
    intvls = [10,5,1]
    sizes = [500,1448]
    seq = 0

    print('start: '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
    for size in sizes:
        for intvl in intvls:
            print('intvl: %f' % intvl)
            seq += 1
            print('tcpdump_tshark: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
            out_filename = 'loss_%s_%s_%s_%02d_%.2f_%d_%s.pcap' % (socket.gethostname(),role,rem_hn,seq,intvl,size,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
            p = sp.Popen(shlex.split('tcpdump -w %s -i %s -n host %s and tcp port 80' % (os.path.join(out_dir,out_filename),intf,rem_ip)))
            if role == 'client':
                sp.call(shlex.split('%s/sanity_test/sender_client 169.235.31.181' % os.path.expanduser('~')))
            elif role == 'server':
                sp.call(shlex.split('%s/sanity_test/sender_server %d 2400 1 %f' % (os.path.expanduser('~'),size,intvl)))
            #tshark(out_dir,out_filename) 
            time.sleep(2)
            p.terminate()
            p.kill()
            sp.call('ps -ef | grep tcpdump;ls -hl %s' % os.path.join(out_dir,out_filename),shell=True)
            print('tcpdump_tshark: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
            time.sleep(120) 

if __name__ == '__main__':
    intf = sys.argv[1]
    rem_ip = sys.argv[2]
    rem_hn = sys.argv[3]
    role = sys.argv[4]
    shift = sys.argv[5]
    sess_intvl = sys.argv[6]

    if os.geteuid():
        print("You need root permissions to do this!")
        sys.exit(1)

    out_dir = os.path.expanduser('~/packet_trace/sched_sender_') + datetime.datetime.utcnow().strftime('%m%d%H%Mutc')
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    sched = BlockingScheduler(timezone=pytz.utc)
    seq = 0
#    start = datetime.datetime.strptime('2018-10-28 %s:30:00' % hour,'%Y-%m-%d %H:%M:%S') 
    # start_str = '1028%s00' % hour   
    start = datetime.datetime.utcnow() + datetime.timedelta(seconds=10)
    end   = start + datetime.timedelta(minutes=119)
    intvl = 3300
#    sched.add_job(tcpdump_tshark, 'interval', args=[out_dir,intf,rem_ip,rem_hn,role],seconds=620,start_date=start+datetime.timedelta(seconds=intvl),end_date=start+datetime.timedelta(hours=2))
    if role == 'client':    
        sched.add_job(client_sender, 'interval', seconds=intvl, start_date=start+datetime.timedelta(seconds=5+shift), end_date=end)
        sched.add_job(tcpdump_tshark, 'interval', args=[out_dir,intf,rem_ip,rem_hn,role,500],seconds=sess_intvl+60,start_date=start+datetime.timedelta(seconds=shift),end_date=start+datetime.timedelta(hours=1))
        # sched.add_job(client_curl, 'date', run_date=start)
    elif role == 'server':
        sched.add_job(server_sender, 'date', run_date=start+datetime.timedelta(seconds=5), args=[500,sess_intvl])
        sched.add_job(tcpdump_tshark, 'interval', args=[out_dir,intf,rem_ip,rem_hn,role,500],seconds=sess_intvl+60,start_date=start,end_date=start+datetime.timedelta(hours=1))


#        sched.add_job(server_sender, 'date', run_date=start+datetime.timedelta(seconds=intvl), args=[1440])

    #Schedule job_function to be called every two hours     
   
    # sched.add_job(tcpdump_tshark, 'interval', seconds=2, run_date='2018-10-26 %s:00:00' % hour, args=[out_dir,intf,rem_ip,rem_hn,role])
    # if role == 'client':    
    #     sched.add_job(client_sender, 'date', run_date='2018-10-26 %s:00:01' % hour)
    # elif role == 'server':
    #     sched.add_job(server_sender, 'date', run_date='2018-10-26 %s:00:00' % hour)
    
    sched.start()


    # def server_tcpdump():
    # print('server_tcpdump: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
    # run_cmd_shell_wtimer('sudo tcpdump -w %s/terran_http_server_calculate_loss_rate_%s.pcap -i eth1 -n host 39.104.139.16 ' % (out_dir,datetime.datetime.utcnow().strftime('%m%d%H%Mutc')),800)
    # print('server_tcpdump: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
