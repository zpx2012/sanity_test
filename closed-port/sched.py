import datetime,time,sys,pytz,os,socket,signal,shlex,logging,csv,subprocess as sp
from apscheduler.schedulers.background import BackgroundScheduler


def mtr(ip,st,src_p,dst_p):
    print '\nmtr:',datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'),'\n'
    cmd = 'bash -c "sudo ~/sanity_test/mtr-modified/mtr -zwnr4A -P %s -L %s -c 60 %s 2>&1 | tee -a ~/sanity_test/rs/mtrack_$(hostname)_%s_%s_%s_1_60_%s.txt"' % (dst_p,src_p,ip,ip,src_p,dst_p,st)
    for i in range(5):
        p = sp.Popen(shlex.split(cmd),stdout=sp.PIPE,stderr=sp.PIPE)
        out,err = p.communicate()
        print 'out:\n',out
        print 'err:\n',err
        if 'Broken pipe' not in out+err:
            break

def hping(ip,st,src_p,dst_p):
    cmd = 'bash hping3.sh %s %s %s A 0.1 120'%(ip,dst_p,src_p)
    p = sp.Popen(shlex.split(cmd))
    p.communicate()

def main():
    sched = BackgroundScheduler(timezone=pytz.utc)
    lines = None
    with open(os.path.expanduser(sys.argv[1]), 'r') as inf:
        lines = filter(None, inf.read().splitlines())
    start = datetime.datetime.utcnow() + datetime.timedelta(seconds=30)
    # start = datetime.datetime.strptime(lines[0],'%Y-%m-%d %H:%M:%S')
    # session = 65
    # intvl = 65*

    for i in range(0,len(lines)):
        fields = lines[i].split(',')
        cur_st = start + datetime.timedelta(seconds=65*i)
        sched.add_job(mtr,'interval', args=[fields[0],cur_st.strftime('%Y%m%d%H%M'),fields[1],fields[2]], seconds=650,
                      start_date=cur_st, end_date=cur_st+datetime.timedelta(days=1))
        sched.add_job(hping,'interval', args=[fields[0],cur_st.strftime('%Y%m%d%H%M'),fields[1],fields[2]], seconds=650,
                      start_date=cur_st+datetime.timedelta(seconds=5*65), end_date=cur_st+datetime.timedelta(days=1))
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