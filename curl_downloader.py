import os,sys,time,datetime,socket,urlparse,threading
from subprocess import Popen, PIPE
from time import sleep
from os.path import expanduser
import curl_poll

def run_cmd(cmd):
    try:
        p = Popen(cmd, shell=True)
        p.communicate()
        # print 'stout:\n %s\nsterr:\n %s\n' % (sout, serr)
        # if not sout:
        #     print '#######\n empty stdout' 
        # return sout, serr
    except KeyboardInterrupt:
        input = raw_input('\n\nTerminate the subprocess and exit?(y to exit, n to restart subprocess):')
        if input == 'y':
            p.terminate()
            os._exit(-1)

if __name__ == '__main__':
    if len(sys.argv) < 6:
        print("Usage:\n\tpython curl_downloader.py [URL] [IP] [Tool] [Site]\n\nOptions:\n\tTool: ss, vpn, ssh, from which server\n\tSite: download source, 163 or mit or so")
        sys.exit(-1)
    url = sys.argv[1]
    ip = sys.argv[2]
    sitename = sys.argv[3]
    run_tr = sys.argv[4]
    speed_limit = sys.argv[5]
    run_inter = sys.argv[6]

    decorator = '\n********************************\n'
    print decorator + 'Curl Downloader 1.1.4\nCtrl-C to terminate the program' + decorator + '\n'

    out_dir = expanduser('~/sanity_test_results/')
    output_file_name = out_dir + '_'.join(['curl',socket.gethostname(),sitename,url.split(':')[0],datetime.datetime.utcnow().strftime('%m%d%H%Mutc')]) +'.txt'
    cmd = 'curl -o /dev/null --limit-rate %s --speed-time 120 -LJv4k --resolve \'%s:%d:%s\' \'%s\' 2>&1 | tee -a %s' % (speed_limit,urlparse.urlparse(url).hostname, 443 if 'https' in url else 80, ip, url, output_file_name)

    #traceroute
    if run_tr == '1':
        print 'traceroute'
        os.system('traceroute -A {} > {}'.format(ip,output_file_name.replace('curl','tr')),)

    num_tasks = 1 
    while True:
        with open(output_file_name,'a') as f:
            f.writelines('\n%s Task : %d\n %s' % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S +0000'), num_tasks, cmd))
        run_cmd(cmd)
        # if run_inter == '1':
        #     print 'xxxxxx'
        #     curl_poll.visit_cn_websites_sleep(10,1)
        num_tasks += 1
        print 'sleep before:%s' % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
        sleep(120)
        print 'sleep after:%s' % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S'))
    # nonproxy_modes = ['clean','https']
    # if sys.argv[3] in nonproxy_modes:
    # else:
    #     cmd = 'curl -o /dev/null --limit-rate 1000k --speed-time 1800 -LJ --socks localhost:1080 \'%s\' 2>&1 | tee -a %s' % (url, output_file_name)
