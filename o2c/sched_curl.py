import datetime,time,sys,pytz,os,socket,signal,shlex,logging,csv,subprocess as sp
from apscheduler.schedulers.background import BackgroundScheduler

def curl_timed(ip,hn,st,sec,src_p=None):
    print '\ncurl timed:',datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'),'\n'
    out = '~/sanity_test/rs/curl_$(hostname)_%s_http_%s.txt' % (hn,st)
    cmd = 'bash -c "echo Start: $(date -u +\'%Y-%m-%d %H:%M:%S\') >> {0};curl -LJv4k -o /dev/null --limit-rate 500k -m {1} --speed-time 120 http://{2}/my.mp4 2>&1 | tee -a {0}"'.format(out,str(sec),ip)
    print cmd
    if sp:
        cmd = cmd.replace('-LJv4k','-LJv4k --local-port '+src_p)
    p = sp.Popen(shlex.split(cmd))
    p.communicate()

def nc_listen(sec,src_p):
    print '\nnc:',datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'),'\n'    
    cmd = 'sudo nc -vv -l %d' % (int(src_p) + 1000)
    print cmd
    p = sp.Popen(shlex.split(cmd))
    time.sleep(140)
    os.system('sudo killall nc')

def mtr(ip,hn,st,src_p,dst_p):
    print '\nmtr:',datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'),'\n'
    outfile = os.path.expanduser('~') + "/sanity_test/rs/mtrins_%s_%s_%s_tcp_1_25_%s.txt" % (socket.gethostname(),hn,src_p,st)
    cmd = 'bash -c "sudo ~/sanity_test/mtr-insertion/mtr -zwnr4T -P %s -L %s -c 25 %s"' % (dst_p,src_p,ip)
    print cmd
    for i in range(5):
        p = sp.Popen(shlex.split(cmd),stdout=sp.PIPE,stderr=sp.PIPE)
        out,err = p.communicate()
        print 'out:\n',out
        print 'err:\n',err
        if 'send_inserted_tcp_packet:time out' not in out+err:
            with open(outfile, 'a') as outf:
                outf.writelines(out+'\n'+err)
            break

def gfw_hop(ip,hn,st,src_p,dst_p,start_ttl):
    print '\ngfw:',datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'),'\n'
    cmd = 'cd ~/filter_hop; sudo ./test ' + ' '.join([ip,str(int(dst_p)+1000),src_p,'$(hostname)',hn,start_ttl])
    print cmd
    p = sp.Popen(cmd,shell=True)
    p.communicate()

def main():
    sched = BackgroundScheduler(timezone=pytz.utc)
    lines = None
    with open(os.path.expanduser(sys.argv[1]), 'r') as inf:
        lines = filter(None, inf.read().splitlines())
    # start = datetime.datetime.strptime(lines[0],'%Y-%m-%d %H:%M:%S')
    session = int(sys.argv[2])
    day = int(sys.argv[3])
    role = sys.argv[4]
    # gfw_delay = int(sys.argv[6])

    intvl = session * len(lines)
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
            # sched.add_job(nc_listen, 'interval', args=[40,fields[2]], seconds=intvl,
                #   start_date=cur_st+datetime.timedelta(seconds=session+gfw_delay), end_date=cur_st+datetime.timedelta(days=day))            
        else:
            sched.add_job(mtr,'interval', args=[fields[0],fields[1],cur_st.strftime('%Y%m%d%H%M'),fields[2],fields[3]], seconds=intvl,
                   start_date=cur_st, end_date=cur_st+datetime.timedelta(days=day))
            # sched.add_job(gfw_hop,'interval', args=[fields[0],fields[1],cur_st.strftime('%Y%m%d%H%M'),fields[2],fields[3],fields[5]], seconds=intvl,
                #    start_date=cur_st+datetime.timedelta(seconds=session+gfw_delay+5), end_date=cur_st+datetime.timedelta(days=day))
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

