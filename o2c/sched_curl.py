import datetime,time,sys,pytz,os,socket,signal,shlex,logging,csv,subprocess as sp
from apscheduler.schedulers.background import BackgroundScheduler

def curl_timed(ip,hn,st,sec,src_p=None):
    print '\ncurl timed:',datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'),'\n'
    out = '~/sanity_test/rs/curl_$(hostname)_%s_http_%s.txt' % (hn,st)
    cmd = 'bash -c "echo Start: $(date -u +\"%Y-%m-%d %H:%M:%S\") >> %s;curl -LJv4k -o /dev/null --limit-rate 500k -m %d --speed-time 120 http://%s/my.mp4 2>&1 | tee -a %s"' % (out,sec,ip,out)
    print cmd
    if sp:
        cmd = cmd.replace('-LJv4k','-LJv4k --local-port '+src_p)
    p = sp.Popen(shlex.split(cmd))
    p.communicate()

def mtr(ip,hn,st,src_p,dst_p):
    print '\nmtr:',datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'),'\n'
    cmd = 'bash -c "sudo ~/sanity_test/mtr-insertion/mtr -zwnr4T -P %s -L %s -c 60 %s 2>&1 | tee -a ~/sanity_test/rs/mtrins_$(hostname)_%s_%s_tcp_1_100_%s.txt"' % (dst_p,src_p,ip,hn,src_p,st)
    for i in range(5):
        p = sp.Popen(shlex.split(cmd),stdout=sp.PIPE,stderr=sp.PIPE)
        out,err = p.communicate()
        print 'out:\n',out
        print 'err:\n',err
        if 'send_inserted_tcp_packet:time out' not in out+err:
            break

def main():
    sched = BackgroundScheduler(timezone=pytz.utc)
    lines = None
    with open(os.path.expanduser(sys.argv[1]), 'r') as inf:
        lines = filter(None, inf.read().splitlines())
    # start = datetime.datetime.strptime(lines[0],'%Y-%m-%d %H:%M:%S')
    session = int(sys.argv[2])
    intvl = int(sys.argv[3])
    day = int(sys.argv[4])
    role = sys.argv[5]
    for i in range(0,len(lines)):
        fields = lines[i].split(',')
        # cur_st = datetime.datetime.utcnow() + datetime.timedelta(seconds=10)
        cur_st = datetime.datetime.strptime(fields[4],'%Y-%m-%d %H:%M:%S')
        now = datetime.datetime.utcnow()
        while cur_st-now < datetime.timedelta(seconds=10):
            cur_st += datetime.timedelta(seconds=intvl)
        print(' '.join(['Previous:', fields[4], 'Changed:', cur_st.strftime('%Y-%m-%d %H:%M:%S')]))
        if role == 'c':
            sched.add_job(curl_timed, 'interval', args=[fields[0],fields[1],cur_st.strftime('%Y%m%d%H%M'),session,fields[2]], seconds=intvl,
                  start_date=cur_st, end_date=cur_st+datetime.timedelta(days=day))
        else:
            sched.add_job(mtr,'interval', args=[fields[0],fields[1],cur_st.strftime('%Y%m%d%H%M'),fields[2],fields[3]], seconds=intvl,
                   start_date=cur_st, end_date=cur_st+datetime.timedelta(days=day))

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