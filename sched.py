import datetime,time,sys,pytz
from apscheduler.schedulers.blocking import BlockingScheduler
from utils import run_cmd_wtimer

def client_tcpdump():
    print('client_tcpdump'+datetime.datetime.utcnow())
    run_cmd_wtimer('sudo tcpdump -i eth0 -n host 169.235.31.181 -w ~/packet_trace/hhht_http_client_calculate_loss_rate_%s.pcap' % datetime.datetime.utcnow().strftime('%m%d%H%Mutc'),800)
    print('client_tcpdump'+datetime.datetime.utcnow())

def server_tcpdump():
    print('server_tcpdump'+datetime.datetime.utcnow())
    run_cmd_wtimer('sudo tcpdump -i eth1 -n host 39.104.139.16 -w ~/packet_trace/terran_http_server_calculate_loss_rate_%s.pcap' % datetime.datetime.utcnow().strftime('%m%d%H%Mutc'),800)
    print('server_tcpdump'+datetime.datetime.utcnow())

def client_curl():
    print('client_curl'+datetime.datetime.utcnow())
    time.sleep(20)
    run_cmd_wtimer('curl -o /dev/null --limit-rate 750k --speed-time 120 -LJv4k http://169.235.31.181/my.pcap',600)
    print('client_curl'+datetime.datetime.utcnow())

if __name__ == '__main__':
    side = sys.argv[1]

    sched = BlockingScheduler(timezone=pytz.utc)
    start = '2018-10-19 07:20:00'
    end   = start + datetime.timedelta(days=1)
    if side == 'c':
        # Schedule job_function to be called every two hours
        sched.add_job(client_tcpdump, 'interval', hours=1,start_date=start,end_date=end)
        sched.add_job(client_curl   , 'interval', hours=1,start_date=start,end_date=end)
    elif side == 's':
        sched.add_job(server_tcpdump, 'interval', hours=1,start_date=start,end_date=end)
    
    sched.start()