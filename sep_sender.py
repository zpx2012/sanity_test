import datetime,time,sys,pytz,os,socket,signal,shlex
import subprocess as sp
from apscheduler.schedulers.blocking import BlockingScheduler
from utils import run_cmd_shell_wtimer,run_cmd_wtimer,run_cmd_shell
from sched import tshark

if __name__ == '__main__':
    intf = sys.argv[1]
    rem_ip = sys.argv[2]
    rem_hn = sys.argv[3]
    role = sys.argv[4]
    hour = sys.argv[5]

    out_dir = os.path.expanduser('~/packet_trace/sched_sep_sender_') + datetime.datetime.utcnow().strftime('%m%d%H%Mutc')
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    intvls = [10,5,1,0.1,0.01,0.001]
    sizes = [500,1448]
    for size in sizes:
        for intvl in intvls:
            print('tcpdump_tshark: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
            out_filename = 'loss_%s_%s_%s_http_%s.pcap' % (socket.gethostname(),role,rem_hn,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
            p = sp.Popen(shlex.split('tcpdump -w %s -i %s -n host %s and tcp port 80' % (os.path.join(out_dir,out_filename),intf,rem_ip)))
            if role == 'client':
                sp.call('cd ~/sanity_test;./sender_client 169.235.31.181',shell=True)
            elif role == 'server':
                sp.call('cd ~/sanity_test;sudo ./sender_server %d 1 %f' % (size,intvl), shell=True)
            tshark(out_dir,out_filename) 
            p.kill()
            sp.call('set -v;ps -ef | grep tcpdump',shell=True)
            time.sleep(120)   
 