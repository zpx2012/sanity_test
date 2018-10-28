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
    seq = 0
    for size in sizes:
        print('size: %d' % size)
        for intvl in intvls:
            if intvl == 10 and size == 500:
                continue
            print('intvl: %d' % intvl)
            seq += 1
            print('tcpdump_tshark: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
            out_filename = 'loss_%s_%s_%s_%d_%s.pcap' % (socket.gethostname(),role,rem_hn,seq,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
            p = sp.Popen(shlex.split('tcpdump -w %s -i %s -n host %s and tcp port 80' % (os.path.join(out_dir,out_filename),intf,rem_ip)))
            if role == 'client':
                run_cmd_wtimer('%s/sanity_test/sender_client 169.235.31.181' % os.path.expanduser('~'),601)
            elif role == 'server':
                run_cmd_wtimer('%s/sanity_test/sender_server %d 1 %f' % (os.path.expanduser('~'),size,intvl),601)
            tshark(out_dir,out_filename) 
            p.terminate()
            p.kill()
            sp.call('killall tcpdump;ps -ef | grep tcpdump;ls -hl %s' % os.path.join(out_dir,out_filename),shell=True)
            time.sleep(120)   
 