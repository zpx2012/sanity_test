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

    if role == 'client':
        while datetime.datetime.utcnow() < datetime.datetime(2018, 10, 29, 16, 10, 0, 0):
            pass
    elif role == 'server':
        while datetime.datetime.utcnow() < datetime.datetime(2018, 10, 29, 16, 0, 0, 0):
            pass
    print('start: '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))

    intvls = [10,5,1]
    sizes = [500,1448]
    seq = 0

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
                sp.call(shlex.split('%s/sanity_test/sender_server %d 1 %f' % (os.path.expanduser('~'),size,intvl)))
            #tshark(out_dir,out_filename) 
            time.sleep(2)
            p.terminate()
            p.kill()
            sp.call('ps -ef | grep tcpdump;ls -hl %s' % os.path.join(out_dir,out_filename),shell=True)
            time.sleep(120)   
 