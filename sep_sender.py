import datetime,time,sys,pytz,os,socket,signal
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

    for
    print('tcpdump_tshark: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
    out_filename = 'loss_%s_%s_%s_http_%s.pcap' % (socket.gethostname(),role,remote_hostname,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
    run_cmd_shell_wtimer('sudo tcpdump -w %s -i %s -n host %s and tcp port 80' % (os.path.join(out_dir,out_filename),interface,remote_ip),7200)
    tshark(out_dir,out_filename)    
 