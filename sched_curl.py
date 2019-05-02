import datetime,time,sys,pytz,os,socket,signal,shlex,logging,csv,subprocess as sp
from apscheduler.schedulers.background import BackgroundScheduler

def curl_timed(ip,hn,st,sec,src_p=None,dst_p=None):
    print '\ncurl timed:',datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'),'\n'
    cmd = 'bash -c "curl -LJv4k -o /dev/null --limit-rate 500k -m %d --speed-time 120 http://%s/my.pcap 2>&1 | tee -a ~/sanity_test/rs/curl_$(hostname)_%s_http_%s.txt"' % (sec,ip,hn,st)
    if src_p:
        cmd = cmd.replace('-LJv4k','-LJv4k --local-port '+src_p)
    p = sp.Popen(shlex.split(cmd))
    p.communicate()

def curl_https_timed(ip,hn,st,sec,src_p=None,dst_p=None):
    print '\ncurl https timed:',datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'),'\n'
    cmd = 'bash -c "curl -LJv4k -o /dev/null --limit-rate 500k -m %d --speed-time 120 https://%s/my.pcap 2>&1 | tee -a ~/sanity_test/rs/curl_$(hostname)_%s_https_%s.txt"' % (sec,ip,hn,st)
    if src_p:
        cmd = cmd.replace('-LJv4k','-LJv4k --local-port '+src_p)
    p = sp.Popen(shlex.split(cmd))
    p.communicate()

def curl_ss_timed(ip,hn,st,sec,src_p=None,dst_p=None):
    print '\ncurl ss timed:',datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'),'\n'
    cmd = 'bash -c "curl -LJv4k -o /dev/null --limit-rate 500k -m %d --speed-time 120 --socks localhost:%s http://%s/my.pcap 2>&1 | tee -a ~/sanity_test/rs/curl_$(hostname)_%s_ss_%s.txt"' % (sec,dst_p,ip,hn,st)
    if src_p:
        cmd = cmd.replace('-LJv4k','-LJv4k --local-port '+src_p)
    p = sp.Popen(shlex.split(cmd))
    p.communicate()

def iperf_timed(ip,hn,st,sec,src_p=None,dst_p=None):
    print '\niperf timed:',datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'),'\n'
    cmd = 'bash -c "iperf3 -c %s -p %s -b 4M -f K -t %s -4VR --logfile %s/sanity_test/rs/iperf3_$(hostname)_%s_%s_%s.txt"'%(ip,dst_p,sec,os.path.expanduser('~'),hn,dst_p,st)
    p = sp.Popen(shlex.split(cmd))
    p.communicate()

def mtr(ip,hn,st,src_p,dst_p):
    print '\nmtr:',datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'),'\n'
    cmd = 'bash -c "sudo ~/sanity_test/mtr-insertion/mtr -zwnr4T -P %s -L %s -c 60 %s 2>&1 | tee -a ~/sanity_test/rs/mtrins_$(hostname)_%s_%s_%s_1_60_%s.txt"' % (dst_p,src_p,ip,hn,src_p,dst_p,st)
    for i in range(5):
        p = sp.Popen(shlex.split(cmd),stdout=sp.PIPE,stderr=sp.PIPE)
        out,err = p.communicate()
        print 'out:\n',out
        print 'err:\n',err
        if 'send_inserted_tcp_packet:time out' not in out+err:
            break

def tcpdump_ip(ip,hn):
    cmd = 'bash %s/sanity_test/prot/tcpdump_iponly.sh %s %s' % (os.path.expanduser('~'),ip,hn)
    p = sp.Popen(shlex.split(cmd),preexec_fn=os.setpgrp)
    time.sleep(86400)
    os.killpg(os.getpgid(p.pid), signal.SIGKILL)
    os.system('set -v;sudo killall -9 %s' % cmds[1] if cmds[0] == 'sudo' else cmds[0])
    print('%d killed' % p.pid)

def main():
    sched = BackgroundScheduler(timezone=pytz.utc)
    lines = None
    with open(os.path.expanduser(sys.argv[1]), 'r') as inf:
        lines = filter(None, inf.read().splitlines())
    # start = datetime.datetime.strptime(lines[0],'%Y-%m-%d %H:%M:%S')
    session = int(sys.argv[2])
    break_between_jobs = sys.argv[3]
    intvl = int(sys.argv[4])
    day = int(sys.argv[5])
    role = sys.argv[6]

    funcs = [curl_timed,curl_https_timed,curl_ss_timed,iperf_timed]
    for i in range(0,len(lines)):
        fields = lines[i].split(',')
        # cur_st = datetime.datetime.utcnow() + datetime.timedelta(seconds=10)
        cur_st = datetime.datetime.strptime(fields[4],'%Y-%m-%d %H:%M:%S')
        if role == 'c':
            sched.add_job(funcs[i%4], 'interval', args=[fields[0],fields[1],cur_st.strftime('%Y%m%d%H%M'),session,fields[2],fields[3]], seconds=intvl,
                  start_date=cur_st, end_date=cur_st+datetime.timedelta(days=day))
            if i % 4 == 0:
                sched.add_job(tcpdump_ip, 'date', run_date=cur_st, args=[fields[0],fields[1]])
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