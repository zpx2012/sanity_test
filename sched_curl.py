import datetime,time,sys,pytz,os,socket,signal,shlex,logging,csv,subprocess as sp
from apscheduler.schedulers.background import BackgroundScheduler

def curl_timed(ip,hn,st,sec):
    print '\n',datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'),'\n'
    cmd = 'bash -c "curl -LJv4k -o /dev/null --limit-rate 500k -m %d --speed-time 120 http://%s/my.pcap 2>&1 | tee -a ~/sanity_test/rs/curl_$(hostname)_%s_http_%s.txt"' % (sec,ip,hn,st)
    p = sp.Popen(shlex.split(cmd))
    p.communicate()

def main():
    sched = BackgroundScheduler(timezone=pytz.utc)
    lines = None
    with open(os.path.expanduser('~/sanity_test/con2con/data/%s.csv'%socket.gethostname()), 'r') as inf:
    # with open('AUS-AWS.csv', 'r') as inf:
        lines = filter(None, inf.read().splitlines())
    start = datetime.datetime.strptime(lines[0],'%Y-%m-%d %H:%M:%S')
    intvl = 20
    for i in range(1,len(lines)):
        fields = lines[i].split(',')
        cur_st = start + datetime.timedelta(seconds=i * intvl)
        sched.add_job(curl_timed, 'interval', args=[fields[0],fields[1],cur_st.strftime('%Y%m%d%H%M'),intvl], minutes=10,
                  start_date=cur_st, end_date=cur_st+datetime.timedelta(days=3))

    sched.start()
    print('Press Ctrl+{0} to exit'.format('Break' if os.name == 'nt' else 'C'))

    try:
        # This is here to simulate application activity (which keeps the main thread alive).
        while True:
            time.sleep(2)
    except (KeyboardInterrupt, SystemExit):
        # Not strictly necessary if daemonic mode is enabled but should be done if possible
        sched.shutdown()

if __name__ == '__main__':
    main()