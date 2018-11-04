import datetime,time,sys,pytz,os,socket,signal,shlex
import subprocess as sp
from apscheduler.schedulers.blocking import BlockingScheduler
from utils import run_cmd_shell_wtimer,run_cmd_wtimer,run_cmd_shell

def tshark(dir,filename):
    cmd = 'cd %s;f=%s;tshark -r $f -Tfields -o tcp.relative_sequence_numbers:FALSE -e ip.id -e tcp.srcport -e tcp.dstport -e tcp.seq -e tcp.ack -e tcp.options.timestamp.tsecr -e tcp.options.timestamp.tsval > ${f%%.*}_1.txt;sort ${f%%.*}_1.txt > ${f%%.*}.tshark;rm ${f%%.*}_1.txt' % (dir,filename)
    run_cmd_shell(cmd)


def tshark_capture(out_dir,interface,remote_ip,remote_hostname,port,role,duration):
    global seq
    print('tshark: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
    out_filename = 'loss_%s_%s_%s_%02d_%s.tshark' % (socket.gethostname(),role,remote_hostname,seq,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
    tshark_cmd = 'tshark -i %s -f \'host %s and tcp port %s\' -Tfields -o tcp.relative_sequence_numbers:FALSE -e ip.id -e tcp.srcport -e tcp.dstport -e tcp.seq -e tcp.ack -e tcp.options.timestamp.tsecr -e tcp.options.timestamp.tsval' % (interface,remote_ip,port)
    output,err = '',''
    try:
        with open(os.path.join(out_dir,out_filename),'w') as f:
            p = sp.Popen(shlex.split(tshark_cmd),stdout=f)
            output,err = p.communicate(timeout=duration)#, stderr=sp.STDOUT
    except sp.TimeoutExpired:
        print('\n\n--------------\ncatch TimeoutExpired. Killed\n-------------\n')
        p.terminate()
        p.kill()
    print('stdout:%s\nstderr:%s\n' % (output,err))
    print("output len:%d" % len(output.splitlines()))
    
    with open(os.path.join(out_dir,out_filename),'r') as f:
        print("f: %s" % f.readlines())
    seq += 1
    print('tshark: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')+'\n')

intvls = [10,5,1,0.1,0.01]

def tcpdump_tshark(out_dir,interface,remote_ip,remote_hostname,port,role,size,duration,flag):
    global seq
    print('tcpdump_tshark: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
    out_filename = 'loss_%s_%s_%s_%d_%02d_%.2f_%d_%s.pcap' % (socket.gethostname(),role,remote_hostname,port,seq,intvls[seq%len(intvls)],size,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
    run_cmd_wtimer('tcpdump -w %s -i %s -n host %s and tcp port %d' % (os.path.join(out_dir,out_filename),interface,remote_ip,port),duration)
    if flag == 1:
        seq += 1
    #tshark(out_dir,out_filename)    
    sp.call('ls -hl %s' % os.path.join(out_dir,out_filename))
    print('tcpdump_tshark: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')+'\n')

def tcpdump_icmp(out_dir,interface,remote_ip,remote_hostname):
    global seq
    print('tcpdump_icmp: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
    out_filename = 'loss_%s_%s_%02d_%s_size.pcap' % (socket.gethostname(),remote_hostname,seq,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
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
    print(sess_intvl)
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
            if role == 'client':
                print('tcpdump curl: client start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
                outfile_dcurl = 'loss_%s_%s_%s_http_%02d_%.2f_%d_%s.pcap' % (socket.gethostname(),role,rem_hn,seq,intvl,size,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
                p_dcurl = sp.Popen(shlex.split('tcpdump -w %s -i %s -n host %s and tcp port 20000' % (os.path.join(out_dir,outfile_dcurl),intf,rem_ip)))
                time.sleep(5)
            elif role == 'server':
                time.sleep(5)
                print('tcpdump curl: server start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
                outfile_dcurl = 'loss_%s_%s_%s_http_%02d_%.2f_%d_%s.pcap' % (socket.gethostname(),role,rem_hn,seq,intvl,size,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
                p_dcurl = sp.Popen(shlex.split('tcpdump -w %s -i %s -n host %s and tcp port 20000' % (os.path.join(out_dir,outfile_dcurl),intf,rem_ip)))
            print('tcpdump sender: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
            outfile_dsender = 'loss_%s_%s_%s_sender_%02d_%.2f_%d_%s.pcap' % (socket.gethostname(),role,rem_hn,seq,intvl,size,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
            p_dsender = sp.Popen(shlex.split('tcpdump -w %s -i %s -n host %s and tcp port 80' % (os.path.join(out_dir,outfile_dsender),intf,rem_ip)))
            if role == 'client':
                sp.call(shlex.split('%s/sanity_test/sender_client 169.235.31.181' % os.path.expanduser('~')))
            elif role == 'server':
                sp.call(shlex.split('%s/sanity_test/sender_server %d 2400 1 %f' % (os.path.expanduser('~'),size,intvl)))
            #tshark(out_dir,outfile) 
            time.sleep(5)
            p_dsender.terminate()
            p_dsender.kill()
            if role == 'client':
                time.sleep(5)
                print('tcpdump curl: client end'+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
                p_dcurl.terminate()
                p_dcurl.kill()
            elif role == 'server':
                print('tcpdump curl: server end'+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
                p_dcurl.terminate()
                p_dcurl.kill()
                time.sleep(5)
            sp.call('ps -ef | grep tcpdump;ls -hl %s;ls -hl %s' % (os.path.join(out_dir,outfile_dcurl),os.path.join(out_dir,outfile_dsender)),shell=True)
            print('tcpdump_tshark: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
            time.sleep(60) 

if __name__ == '__main__':
    intf = sys.argv[1]
    rem_ip = sys.argv[2]
    rem_hn = sys.argv[3]
    role = sys.argv[4]
    shift = int(sys.argv[5])
    sess_intvl = int(sys.argv[6])
    minute = sys.argv[7]

    print(sess_intvl)
    if os.geteuid():
        print("You need root permissions to do this!")
        sys.exit(1)

    out_dir = os.path.expanduser('~/packet_trace/sched_tshark_') + datetime.datetime.utcnow().strftime('%m%d%H%Mutc')
    if not os.path.exists(os.path.expanduser('~/packet_trace')):
        os.makedirs(os.path.expanduser('~/packet_trace'))
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    sched = BlockingScheduler(timezone=pytz.utc)
    seq = 0
    # start = datetime.datetime.strptime('2018-11-04 08:%s:00' % minute,'%Y-%m-%d %H:%M:%S') 
    # start_str = '1028%s00' % hour   
    start = datetime.datetime.utcnow() + datetime.timedelta(seconds=3)
    end   = start + datetime.timedelta(days=1)
    # intvl = 3300
#    sched.add_job(tcpdump_tshark, 'interval', args=[out_dir,intf,rem_ip,rem_hn,role],seconds=620,start_date=start+datetime.timedelta(seconds=intvl),end_date=start+datetime.timedelta(hours=2))
    if role == 'client':    
        # sched.add_job(client_sender, 'date', run_date=start+datetime.timedelta(seconds=shift))
        sched.add_job(tshark_capture, 'interval', args=[out_dir,intf,rem_ip,rem_hn,80,role,2+sess_intvl+2],minutes=1,start_date=start+datetime.timedelta(seconds=shift),end_date=end)
        # sched.add_job(tcpdump_tshark, 'interval', args=[out_dir,intf,rem_ip,rem_hn,80,role,500,5+sess_intvl+20,1],seconds=sess_intvl+60,start_date=start+datetime.timedelta(seconds=shift-5),end_date=end)
        # sched.add_job(tcpdump_tshark, 'interval', args=[out_dir,intf,rem_ip,rem_hn,20000,role,500,5+sess_intvl+20+10,0],seconds=sess_intvl+60-10,start_date=start+datetime.timedelta(seconds=shift-10),end_date=start+datetime.timedelta(hours=1))
        # sched.add_job(sep_sender, 'date', args=[intf,rem_ip,rem_hn,role], run_date=start+datetime.timedelta(seconds=5+shift))#+datetime.timedelta(minutes=90))
        # sched.add_job(client_curl, 'date', run_date=start)
    elif role == 'server':
        sched.add_job(tshark_capture, 'interval', args=[out_dir,intf,rem_ip,rem_hn,80,role,sess_intvl],minutes=1,start_date=start+datetime.timedelta(seconds=2),end_date=end)

        # sched.add_job(server_sender, 'date', run_date=start, args=[500,sess_intvl])
        # sched.add_job(tcpdump_tshark, 'interval', args=[out_dir,intf,rem_ip,rem_hn,80,role,500,5+sess_intvl+20,1],seconds=sess_intvl+60,start_date=start+datetime.timedelta(seconds=-5),end_date=start+datetime.timedelta(hours=1))
        # sched.add_job(tcpdump_tshark, 'interval', args=[out_dir,intf,rem_ip,rem_hn,20000,role,500,5+sess_intvl+20,0],seconds=sess_intvl+60,start_date=start+datetime.timedelta(seconds=-5),end_date=start+datetime.timedelta(hours=1))
        # sched.add_job(sep_sender, 'date', args=[intf,rem_ip,rem_hn,role], run_date=start+datetime.timedelta(seconds=5))#+datetime.timedelta(minutes=90))

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
