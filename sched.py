import datetime,time,sys,pytz
from apscheduler.schedulers.blocking import BlockingScheduler
from utils import run_cmd_shell_wtimer,run_cmd_wtimer

def client_tcpdump():
    print('client_tcpdump'+datetime.datetime.utcnow().strftime('%m%d%H%M'))
    run_cmd_shell_wtimer('sudo tcpdump -w ~/packet_trace/hhht_http_client_calculate_loss_rate_%s.pcap -i eth0 -n host 169.235.31.181 ' % datetime.datetime.utcnow().strftime('%m%d%H%Mutc'),5)
    print('client_tcpdump'+datetime.datetime.utcnow().strftime('%m%d%H%M'))

def server_tcpdump():
    print('server_tcpdump'+datetime.datetime.utcnow().strftime('%m%d%H%M'))
    run_cmd_shell_wtimer('sudo tcpdump -w ~/packet_trace/terran_http_server_calculate_loss_rate_%s.pcap -i eth1 -n host 39.104.139.16 ' % datetime.datetime.utcnow().strftime('%m%d%H%Mutc'),5)
    print('server_tcpdump'+datetime.datetime.utcnow().strftime('%m%d%H%M'))

def client_curl():
    print('client_curl'+datetime.datetime.utcnow().strftime('%m%d%H%M'))
    time.sleep(20)
    run_cmd_wtimer('curl -o /dev/null --limit-rate 750k --speed-time 120 -LJv4k http://169.235.31.181/my.pcap',600)
    print('client_curl'+datetime.datetime.utcnow().strftime('%m%d%H%M'))

if __name__ == '__main__':
    side = sys.argv[1]

    sched = BlockingScheduler(timezone=pytz.utc)
    start = datetime.datetime.utcnow() + datetime.timedelta(seconds=10)
    # start = datetime.datetime.strptime('2018-10-19 07:50:00','%Y-%m-%d %H:%M:%S')
    end   = start + datetime.timedelta(days=1)
    if side == 'c':
        # Schedule job_function to be called every two hours
        sched.add_job(client_tcpdump, 'interval', hours=1,start_date=start,end_date=end)
        sched.add_job(client_curl   , 'interval', hours=1,start_date=start,end_date=end)
    elif side == 's':
        sched.add_job(server_tcpdump, 'interval', hours=1,start_date=start,end_date=end)
    
    sched.start()