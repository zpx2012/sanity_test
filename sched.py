import datetime,time,sys,pytz,os,socket,signal,shlex,logging,csv
import subprocess as sp
from urllib.parse import urlparse
from apscheduler.schedulers.background import BackgroundScheduler
from utils import run_cmd_shell_wtimer,run_cmd_wtimer,run_cmd_shell
from curl_poll import curl_poll_csv

# 

def tcpdump_1116(out_dir,remote_ip,remote_hostname,port,duration,role):
    print('tcpdump_1116: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
    if role == 'c':
        out_filename = 'loss_%s_%s_%s_%s_client.pcap' % (socket.gethostname(),remote_hostname,'http' if port == '80' else 'ss',datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
    elif role == 's':
        out_filename = 'loss_%s_%s_%s_%s_server.pcap' % (remote_hostname,socket.gethostname(),'http' if port == '80' else 'ss',datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
    else:
        return 

    intf = sp.check_output('ip route | grep default | sed -e "s/^.*dev.//" -e "s/.proto.*//"',shell=True)
    run_cmd_wtimer('tcpdump -w %s -s 96 -i %s -n host %s and tcp port %d' % (os.path.join(out_dir,out_filename),intf,remote_ip,port),duration)
    sp.call('ls -hl %s' % os.path.join(out_dir,out_filename),shell=True)
    print('tcpdump_1116: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')+'\n')


def main():
    # intf = 'scripts/1125_aliyun.csv'
    # rem_ip = '169.235.31.181'#sys.argv[1]
    # rem_hn = 'terran'#sys.argv[2]
    infile = sys.argv[2]
    role = sys.argv[3]
    # shift = int(sys.argv[5])
    # sess_intvl = int(sys.argv[6])
    # minute = sys.argv[3]

    # logging.basicConfig()
    # logging.getLogger('apscheduler').setLevel(logging.DEBUG)
    
    if os.geteuid():
        print("You need root permissions to do this!")
        sys.exit(1)

    out_dir = os.path.expanduser('~/packet_trace/sched_mtr_') + datetime.datetime.utcnow().strftime('%m%d%H%Mutc')
    if not os.path.exists(os.path.expanduser('~/packet_trace')):
        os.makedirs(os.path.expanduser('~/packet_trace'))
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)
 
    with open(os.path.expanduser(infile),'r') as f:         
        ip_hn_list = list(csv.reader(f))

    sched = BackgroundScheduler(timezone=pytz.utc)
    # seq = 0
    start = datetime.datetime.utcnow()# + datetime.timedelta(seconds=3)
    start = start.replace(minute=start.minute+2,second=0,microsecond=0)
    # start = datetime.datetime.strptime('2018-11-09 %s:00' % minute,'%Y-%m-%d %H:%M:%S') 
    # start_str = '1028%s00' % hour   
    end   = start + datetime.timedelta(days=1)

    dur,start_offset = 0,0
    if role == 'c':
        dur,start_offset = 2+15+2,0
    elif role == 's':
        dur,start_offset = 15,2
    else:
        print('Role not identified')
        return

    for i,line in enumerate(ip_hn_list):
        sched.add_job(tcpdump_1116, 'interval', args=[out_dir,line[1],line[0],80,dur,role],minutes=1,start_date=start+start_offset,end_date=end)
        sched.add_job(tcpdump_1116, 'interval', args=[out_dir,line[1],line[0],line[2],dur,role],minutes=1,start_date=start+start_offset,end_date=end)


    # if role == 'client':    
    #     for i,line in enumerate(ip_hn_list):
    #         output_file_name = out_dir + '_'.join(['curl',socket.gethostname(),line[2],line[0].split(':')[0],datetime.datetime.utcnow().strftime('%m%d%H%Mutc')]) +'.txt'
    #         sched.add_job(curl_vultr,'interval', args=[line,output_file_name],minutes=1,start_date=start+datetime.timedelta(seconds=10*i+2), end_date=end)
            # sched.add_job(tshark_capture, 'interval', args=[out_dir,intf,line[1],line[2],80,role,2+10+2],minutes=1,start_date=start+datetime.timedelta(seconds=10*i),end_date=end)
    # elif role == 'server':
    #     for i,line in enumerate(ip_hn_list):
    #         if socket.gethostname() == line[2]:
    #             sched.add_job(tshark_capture, 'interval', args=[out_dir,intf,'39.108.98.242','sz1-aliyun',80,role,10],minutes=1,start_date=start+datetime.timedelta(seconds=2+10*i),end_date=end)
  
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


    # def server_tcpdump():
    # print('server_tcpdump: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
    # run_cmd_shell_wtimer('sudo tcpdump -w %s/terran_http_server_calculate_loss_rate_%s.pcap -i eth1 -n host 39.104.139.16 ' % (out_dir,datetime.datetime.utcnow().strftime('%m%d%H%Mutc')),800)
    # print('server_tcpdump: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
        # sched.add_job(server_sender, 'date', run_date=start, args=[500,sess_intvl])
        # sched.add_job(tcpdump_tshark, 'interval', args=[out_dir,intf,rem_ip,rem_hn,80,role,500,5+sess_intvl+20,1],seconds=sess_intvl+60,start_date=start+datetime.timedelta(seconds=-5),end_date=start+datetime.timedelta(hours=1))
        # sched.add_job(tcpdump_tshark, 'interval', args=[out_dir,intf,rem_ip,rem_hn,20000,role,500,5+sess_intvl+20,0],seconds=sess_intvl+60,start_date=start+datetime.timedelta(seconds=-5),end_date=start+datetime.timedelta(hours=1))
        # sched.add_job(sep_sender, 'date', args=[intf,rem_ip,rem_hn,role], run_date=start+datetime.timedelta(seconds=5))#+datetime.timedelta(minutes=90))

#        sched.add_job(server_sender, 'date', run_date=start+datetime.timedelta(seconds=intvl), args=[1440])

    #Schedule job_function to be called every two hours     
   
    # sched.add_job(tcpdump_tshark, 'interval', seconds=2, run_date='2018-10-26 %s:00:00' % hour, args=[out_dir,intf,rem_ip,rem_hn,role])
    # if role == 'client':    
    #     sched.add_job(client_sender, 'date', run_date='2018-10-26 %s:00:01' % hour)
    # elif role == 'server':
    #     sched.add_job(server_sender, 'date', run_date='2018-10-26 %s:00:00' % hour)
  
          # sched.add_job(client_sender, 'date', run_date=start+datetime.timedelta(seconds=shift))
        # sched.add_job(tcpdump_tshark, 'interval', args=[out_dir,intf,rem_ip,rem_hn,80,role,500,5+sess_intvl+20,1],seconds=sess_intvl+60,start_date=start+datetime.timedelta(seconds=shift-5),end_date=end)
        # sched.add_job(tcpdump_tshark, 'interval', args=[out_dir,intf,rem_ip,rem_hn,20000,role,500,5+sess_intvl+20+10,0],seconds=sess_intvl+60-10,start_date=start+datetime.timedelta(seconds=shift-10),end_date=start+datetime.timedelta(hours=1))
        # sched.add_job(sep_sender, 'date', args=[intf,rem_ip,rem_hn,role], run_date=start+datetime.timedelta(seconds=5+shift))#+datetime.timedelta(minutes=90))
        # sched.add_job(client_curl, 'date', run_date=start)

# def tshark(dir,filename):
#     cmd = 'cd %s;f=%s;tshark -r $f -Tfields -o tcp.relative_sequence_numbers:FALSE -e ip.id -e tcp.srcport -e tcp.dstport -e tcp.seq -e tcp.ack -e tcp.options.timestamp.tsecr -e tcp.options.timestamp.tsval > ${f%%.*}_1.txt;sort ${f%%.*}_1.txt > ${f%%.*}.tshark;rm ${f%%.*}_1.txt' % (dir,filename)
#     run_cmd_shell(cmd)


# def tshark_capture(out_dir,interface,remote_ip,remote_hostname,port,role,duration):
#     print('tshark: start %s %s' % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'),remote_ip))
#     out_filename = 'loss_%s_%s_%s_%s.tshark' % (socket.gethostname(),role,remote_hostname,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
#     tshark_cmd = 'tshark -i %s -f \'host %s and tcp port %s\' -Tfields -o tcp.relative_sequence_numbers:FALSE -e ip.id -e tcp.srcport -e tcp.dstport -e tcp.seq -e tcp.ack -e tcp.options.timestamp.tsecr -e tcp.options.timestamp.tsval' % (interface,remote_ip,port)
#     try:
#         with open(os.path.join(out_dir,out_filename),'w') as f:
#             sp.call(shlex.split(tshark_cmd),stdout=f,stderr=sp.PIPE,timeout=duration)
#             # p = sp.Popen(shlex.split(tshark_cmd),stdout=f)
#             # output,err = p.communicate(timeout=duration)#, stderr=sp.STDOUT
#     except sp.TimeoutExpired:
#         print('catch TimeoutExpired. Killed.')
    
#     with open(os.path.join(out_dir,out_filename),'r') as f:
#         print("f: %d" % len(f.readlines()))
#     sp.call('rm -rf /tmp/wireshark_*.pcapng',shell=True)
#     print('tshark: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')+'\n')

# intvls = [10,5,1,0.1,0.01]

# def tcpdump_tshark(out_dir,interface,remote_ip,remote_hostname,port,role,size,duration):
#     # global seq
#     print('tcpdump_tshark: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
#     out_filename = 'loss_%s_%s_%s_%d_%02d_%.2f_%d_%s.pcap' % (socket.gethostname(),role,remote_hostname,port,seq,intvls[seq%len(intvls)],size,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
#     run_cmd_wtimer('tcpdump -w %s -i %s -n host %s and tcp port %d' % (os.path.join(out_dir,out_filename),interface,remote_ip,port),duration)
#     # if flag == 1:
#     #     seq += 1
#     #tshark(out_dir,out_filename)    
#     sp.call('ls -hl %s' % os.path.join(out_dir,out_filename))
#     print('tcpdump_tshark: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')+'\n')

# def tcpdump_icmp(out_dir,interface,remote_ip,remote_hostname):
#     global seq
#     print('tcpdump_icmp: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
#     out_filename = 'loss_%s_%s_%02d_%s_size.pcap' % (socket.gethostname(),remote_hostname,seq,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
#     run_cmd_wtimer('tcpdump -w %s -i %s -n host %s and icmp' % (os.path.join(out_dir,out_filename),interface,remote_ip),615)
#     seq += 1
#     #tshark(out_dir,out_filename)    

#     print('tcpdump_icmp: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')+'\n')




# def loss_test(out_dir,interface,remote_ip,remote_hostname,role,seq):
#     out_filename = 'loss_%s_%s_%s_%d_%s.pcap' % (socket.gethostname(),role,remote_hostname,seq,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
#     tshark_cmd = 'tshark -i %s -f \'host %s  and tcp port %s\' -Tfields -o tcp.relative_sequence_numbers:FALSE -e ip.id -e tcp.srcport -e tcp.dstport -e tcp.seq -e tcp.ack -e tcp.options.timestamp.tsecr -e tcp.options.timestamp.tsval > %s'
#     p_td = sp.Popen(tshark_cmd % (os.path.join(out_dir,out_filename),interface,remote_ip),shell=True, preexec_fn=os.setpgrp)
#     #do something

#     os.killpg(os.getpgid(p_td.pid), signal.SIGTERM)

# # def client_curl():
# #     global start_str
# #     print('client_curl: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
# #     run_cmd_shell_wtimer('script -aqf -c \'while true;do curl -o /dev/null --limit-rate 500k --speed-time 120 -LJv4k http://169.235.31.181:20000/my.pcap; sleep 10;done\' %s/curl_%s_terran_loss_test_%s.txt'% (out_dir,socket.gethostname(),start_str),7200)
# #     print('client_curl: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')+'\n')

# def client_sender():
#     sp.call(shlex.split('%s/sanity_test/sender_client 169.235.31.181' % os.path.expanduser('~')))

# def server_sender(size,sess_intvl):
#     print(sess_intvl)
#     sp.call(shlex.split('%s/sanity_test/sender_server %d %d 0' % (os.path.expanduser('~'),size,sess_intvl)))

# def sep_sender(intf,rem_ip,rem_hn,role):
#     intvls = [10,5,1]
#     sizes = [500,1448]
#     seq = 0

#     print('start: '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
#     for size in sizes:
#         for intvl in intvls:
#             print('intvl: %f' % intvl)
#             seq += 1
#             if role == 'client':
#                 print('tcpdump curl: client start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
#                 outfile_dcurl = 'loss_%s_%s_%s_http_%02d_%.2f_%d_%s.pcap' % (socket.gethostname(),role,rem_hn,seq,intvl,size,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
#                 p_dcurl = sp.Popen(shlex.split('tcpdump -w %s -i %s -n host %s and tcp port 20000' % (os.path.join(out_dir,outfile_dcurl),intf,rem_ip)))
#                 time.sleep(5)
#             elif role == 'server':
#                 time.sleep(5)
#                 print('tcpdump curl: server start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
#                 outfile_dcurl = 'loss_%s_%s_%s_http_%02d_%.2f_%d_%s.pcap' % (socket.gethostname(),role,rem_hn,seq,intvl,size,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
#                 p_dcurl = sp.Popen(shlex.split('tcpdump -w %s -i %s -n host %s and tcp port 20000' % (os.path.join(out_dir,outfile_dcurl),intf,rem_ip)))
#             print('tcpdump sender: start '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
#             outfile_dsender = 'loss_%s_%s_%s_sender_%02d_%.2f_%d_%s.pcap' % (socket.gethostname(),role,rem_hn,seq,intvl,size,datetime.datetime.utcnow().strftime('%m%d%H%Mutc'))
#             p_dsender = sp.Popen(shlex.split('tcpdump -w %s -i %s -n host %s and tcp port 80' % (os.path.join(out_dir,outfile_dsender),intf,rem_ip)))
#             if role == 'client':
#                 sp.call(shlex.split('%s/sanity_test/sender_client 169.235.31.181' % os.path.expanduser('~')))
#             elif role == 'server':
#                 sp.call(shlex.split('%s/sanity_test/sender_server %d 2400 1 %f' % (os.path.expanduser('~'),size,intvl)))
#             #tshark(out_dir,outfile) 
#             time.sleep(5)
#             p_dsender.terminate()
#             p_dsender.kill()
#             if role == 'client':
#                 time.sleep(5)
#                 print('tcpdump curl: client end'+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
#                 p_dcurl.terminate()
#                 p_dcurl.kill()
#             elif role == 'server':
#                 print('tcpdump curl: server end'+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
#                 p_dcurl.terminate()
#                 p_dcurl.kill()
#                 time.sleep(5)
#             sp.call('ps -ef | grep tcpdump;ls -hl %s;ls -hl %s' % (os.path.join(out_dir,outfile_dcurl),os.path.join(out_dir,outfile_dsender)),shell=True)
#             print('tcpdump_tshark: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
#             time.sleep(60) 

# def curl_vultr(line,output_file_name):
#     base_cmd = 'script -aqf -c \'curl -o /dev/null --limit-rate %s --speed-time 120 -LJv4k --resolve "%s:%d:%s" "%s"\' %s'
#     cmd = base_cmd % (line[3],urlparse(line[0]).netloc, 443 if line[0].split(':')[0] == 'https' else 80,line[1],line[0],output_file_name)
#     print('curl poll:start %s %s' % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S +0000'), line[1]))
#     run_cmd_wtimer(cmd,int(line[4]))
#     print('curl poll: end '+datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S')+'\n')