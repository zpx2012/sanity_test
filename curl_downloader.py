import os,sys,time,datetime,socket,urlparse,threading,psutil,traceback
from subprocess import Popen, PIPE
from time import sleep
from os.path import expanduser
#import curl_poll


if __name__ == '__main__':
    if len(sys.argv) < 6:
        print("Usage:\n\tpython curl_downloader.py [URL] [IP] [Tool] [Site]\n\nOptions:\n\tTool: ss, vpn, ssh, from which server\n\tSite: download source, 163 or mit or so")
        sys.exit(-1)
    url = sys.argv[1]
    ip = sys.argv[2]
    sitename = sys.argv[3]
    run_tr = sys.argv[4]
    speed_limit = sys.argv[5]
    proxy_port = sys.argv[6]
    local_port = sys.argv[7]

    decorator = '\n********************************\n'
    print decorator + 'Curl Downloader 1.1.4\nCtrl-C to terminate the program' + decorator + '\n'

    out_dir = expanduser('~/sanity_test/rs/')
    output_file_name = out_dir + '_'.join(['curl',socket.gethostname(),sitename,url.split(':')[0] if proxy_port == '0' else 'ss',datetime.datetime.utcnow().strftime('%m%d%H%Mutc')]) +'.txt'
    pid_file_name = output_file_name.replace('curl','pid')

    sl = '--limit-rate %s ' % speed_limit if speed_limit != '0' else ''
    lp = '--local-port %s ' % local_port if local_port != '0' else ''
    if proxy_port == '0':
        cmd = 'curl -o /dev/null %s --speed-time 120 -LJv4k --resolve \'%s:%d:%s\' \'%s\' 2>&1 | tee -a %s' % (sl+lp,urlparse.urlparse(url).hostname, 443 if 'https' in url else 80, ip, url, output_file_name)
    else:
        cmd = 'curl -o /dev/null %s --speed-time 120 -LJv4k --socks localhost:%s \'%s\' 2>&1 | tee -a %s' % (sl+lp,proxy_port,url, output_file_name)

    #traceroute
    if run_tr == '1':
        print 'traceroute'
        os.system('traceroute -A {} > {}'.format(ip,output_file_name.replace('curl','tr')),)

    num_tasks = 1 
    while True:
        with open(output_file_name,'a') as f:
            f.writelines('\n%s Task : %d\n %s' % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S +0000'), num_tasks, cmd))
        try:
            p = Popen(cmd, shell=True,stdout=PIPE,stderr=PIPE)#
            sleep(2)
            if p.poll() == None:
                if proxy_port == '0':
                    children = psutil.Process(os.getpid()).children(recursive=True)
                    curl_pids = None
                    while not curl_pids:
                        curl_pids = [x.pid for x in children if x.name() == 'curl']
                    with open(pid_file_name,'a') as f:
                        f.writelines('%d\n' % curl_pids[0])
                num_tasks += 1
                out,err = p.communicate()
                print 'out:\n%s\nerr:\n%s'%(out,err)
                out_lines = filter(None,out.splitlines())
                if len(out_lines) > 2:
                    speed = [filter(None,l.split(' '))[6] for l in out_lines if l.startswith('100 ')][0]#use avg speed
                    print speed
                    if 'M' in speed or ('k' in speed and int(speed.split('k')[0]) > 500):
                        print 'sleep before:%s' % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
                        sleep(60)
                        print 'sleep after:%s' % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
                        
        except KeyboardInterrupt:
            input = raw_input('\n\nTerminate the subprocess and exit?(y to exit, n to restart subprocess):')
            if input == 'y':
                p.terminate()
                os._exit(-1)
        except:
            print '###\n%s' % traceback.format_exc()


    # nonproxy_modes = ['clean','https']
        # if run_inter == '1':
        #     print 'xxxxxx'
        #     curl_poll.visit_cn_websites_sleep(10,1)