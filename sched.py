import datetime,time,sys,pytz,os,socket
from apscheduler.schedulers.blocking import BlockingScheduler
from utils import run_cmd_shell_wtimer,run_cmd_wtimer,run_cmd_shell

def tshark(dir,filename):
    cmd = 'cd %s;f=%s;tshark -r $f -Tfields -o tcp.relative_sequence_numbers:FALSE -e ip.id -e tcp.srcport -e tcp.dstport -e tcp.seq -e tcp.ack -e tcp.options.timestamp.tsecr -e tcp.options.timestamp.tsval > ${f%%.*}_1.txt;sort ${f%%.*}_1.txt > ${f%%.*}.tshark;rm ${f%%.*}_1.txt' % (dir,filename)
    run_cmd_shell(cmd)

def tcpdump_tshark(out_dir,interface,remote_ip,remote_hostname,role):
    print('tcpdump_tshark: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
    out_filename = 'loss_%s_%s_%s_http_%s.pcap' % (socket.gethostname(),role,remote_hostname,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
    run_cmd_shell_wtimer('sudo tcpdump -w %s -i %s -n host %s' % (os.path.join(out_dir,out_filename),remote_ip),700)
    tshark(out_dir,out_filename)    
    print('tcpdump_tshark: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')+'\n')


def client_curl():
    print('client_curl: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
    run_cmd_wtimer('script -aqf -c \'while true;do curl -o /dev/null --limit-rate 500k --speed-time 120 -LJv4k http://169.235.31.181/sdk-tools-linux-3859397.zip; done\' %s/curl_terran_loss_test_%s.txt'%(out_dir,datetime.datetime.utcnow().strftime('%m%d%H%Mutc')),600)
    print('client_curl: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')+'\n')

if __name__ == '__main__':
    intf = sys.argv[1]
    rem_ip = sys.argv[2]
    rem_hn = sys.argv[3]
    role = sys.argv[4]

    out_dir = os.path.expanduser('~/packet_trace/sched_loss_') + datetime.datetime.utcnow().strftime('%m%d%H%Mutc')
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    sched = BlockingScheduler(timezone=pytz.utc)
    start = datetime.datetime.utcnow() + datetime.timedelta(seconds=10)
    end   = start + datetime.timedelta(days=1)
    # Schedule job_function to be called every two hours     
    sched.add_job(tcpdump_tshark, 'interval', args=[out_dir,intf,rem_ip,rem_hn,role],hours=1,start_date=start,end_date=end)
    if side == 'client':    
        sched.add_job(client_curl, 'interval', hours=1,start_date=start+datetime.timedelta(seconds=10),end_date=end)
    
    sched.start()

    # start = datetime.datetime.strptime('2018-10-19 07:50:00','%Y-%m-%d %H:%M:%S')
    # def server_tcpdump():
    # print('server_tcpdump: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
    # run_cmd_shell_wtimer('sudo tcpdump -w %s/terran_http_server_calculate_loss_rate_%s.pcap -i eth1 -n host 39.104.139.16 ' % (out_dir,datetime.datetime.utcnow().strftime('%m%d%H%Mutc')),800)
    # print('server_tcpdump: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
