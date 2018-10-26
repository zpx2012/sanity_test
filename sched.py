import datetime,time,sys,pytz,os,socket
import subprocess as sp
from apscheduler.schedulers.blocking import BlockingScheduler
from utils import run_cmd_shell_wtimer,run_cmd_wtimer,run_cmd_shell

def tshark(out_dir,interface,remote_ip,remote_hostname,role):
    global seq
    print('tshark: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
    out_filename = 'loss_%s_%s_%s_%d_%s.tshark' % (socket.gethostname(),role,remote_hostname,seq,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
    tshark_cmd = 'sudo tshark -i %s -f host %s and tcp port %s -Tfields -o tcp.relative_sequence_numbers:FALSE -e ip.id -e tcp.srcport -e tcp.dstport -e tcp.seq -e tcp.ack -e tcp.options.timestamp.tsecr -e tcp.options.timestamp.tsval > %s'
    run_cmd_shell_wtimer(tshark_cmd % (interface,remote_ip,80,os.path.join(out_dir,out_filename)),600)
    seq += 1
    print('tshark: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')+'\n')

def tcpdump_tshark(out_dir,interface,remote_ip,remote_hostname,role):
    print('tcpdump_tshark: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
    out_filename = 'loss_%s_%s_%s_http_%s.pcap' % (socket.gethostname(),role,remote_hostname,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
    run_cmd_shell_wtimer('sudo tcpdump -w %s -i %s -n host %s' % (os.path.join(out_dir,out_filename),interface,remote_ip),7200)
    tshark(out_dir,out_filename)    
    print('tcpdump_tshark: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')+'\n')



def loss_test(out_dir,interface,remote_ip,remote_hostname,role,seq):
    out_filename = 'loss_%s_%s_%s_%d_%s.pcap' % (socket.gethostname(),role,remote_hostname,seq,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
    tshark_cmd = 'tshark -i %s -f \'host %s  and tcp port %s\' -Tfields -o tcp.relative_sequence_numbers:FALSE -e ip.id -e tcp.srcport -e tcp.dstport -e tcp.seq -e tcp.ack -e tcp.options.timestamp.tsecr -e tcp.options.timestamp.tsval > %s'
    p_td = sp.Popen(tshark_cmd % (os.path.join(out_dir,out_filename),interface,remote_ip),shell=True, preexec_fn=os.setpgrp)
    #do something

    os.killpg(os.getpgid(p.pid), signal.SIGTERM)

def client_curl():
    global start_str
    print('client_curl: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
    run_cmd_shell_wtimer('script -aqf -c \'while true;do curl -o /dev/null --limit-rate 500k --speed-time 120 -LJv4k http://169.235.31.181:20000/my.pcap; sleep 10;done\' %s/curl_%s_terran_loss_test_%s.txt'% (out_dir,socket.gethostname(),start_str),7200)
    print('client_curl: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')+'\n')

def client_sender():
    os.system('cd ~/sanity_test;./sender_client 169.235.31.181')

def server_sender():
    os.system('cd ~/sanity_test;sudo ./sender_server')

if __name__ == '__main__':
    intf = sys.argv[1]
    rem_ip = sys.argv[2]
    rem_hn = sys.argv[3]
    role = sys.argv[4]
    hour = sys.argv[5]

    if os.geteuid():
        print("You need root permissions to do this!")
        sys.exit(1)

    out_dir = os.path.expanduser('~/packet_trace/sched_sender_') + datetime.datetime.utcnow().strftime('%m%d%H%Mutc')
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    sched = BlockingScheduler(timezone=pytz.utc)
    seq = 0
    #start = datetime.datetime.strptime('2018-10-26 %s:00:00' % hour,'%Y-%m-%d %H:%M:%S') 
    start_str = '1026%s00' % hour   
    start = datetime.datetime.utcnow() + datetime.timedelta(seconds=10)
    end   = start + datetime.timedelta(hours=2)
    sched.add_job(tshark, 'interval', args=[out_dir,intf,rem_ip,rem_hn,role],seconds=602,start_date=start,end_date=end)
    if role == 'client':    
        sched.add_job(client_sender, 'date', run_date=start)
        sched.add_job(client_curl, 'date', run_date=start)
    elif role == 'server':
        sched.add_job(server_sender, 'date', run_date=start)

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
