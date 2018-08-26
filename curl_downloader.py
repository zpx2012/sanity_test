import os,sys,time,datetime,socket,urlparse,threading
from subprocess import Popen, PIPE
from time import sleep
from os.path import expanduser

if __name__ == '__main__':
    if len(sys.argv) < 6:
        print("Usage:\n\tpython curl_downloader.py [URL] [IP] [Tool] [Site]\n\nOptions:\n\tTool: ss, vpn, ssh, from which server\n\tSite: download source, 163 or mit or so")
        sys.exit(-1)
    url = sys.argv[1]
    ip = sys.argv[2]

    out_dir = expanduser('~/sanity_test_results/')
    output_file_name = out_dir + 'curl_' + socket.gethostname() + "_" + sys.argv[3] + "_" + sys.argv[4] + "_" + datetime.datetime.now().strftime("%m%d%H%M")+".txt"

    decorator = '\n********************************\n'
    print decorator + 'Curl Downloader 1.1.4\nCtrl-C to terminate the program' + decorator + '\n'

    #traceroute
    if sys.argv[5] == 1:
        os.system('traceroute -A {} > {}'.format(ip,output_file_name.replace('curl','tr')),)

    nonproxy_modes = ['clean','https']
    if 'https' in url:
        port = 443
    else:
        port = 80
    if sys.argv[3] in nonproxy_modes:
        cmd = 'curl -o /dev/null --limit-rate 750k --speed-time 1800 -LJv4k --resolve \'%s:%d:%s\' \'%s\' 2>&1 | tee -a %s' % (urlparse.urlparse(url).hostname, port, ip, url, output_file_name)
    else:
        cmd = 'curl -o /dev/null --limit-rate 1000k --speed-time 1800 -LJ --socks localhost:1080 \'%s\' 2>&1 | tee -a %s' % (url, output_file_name)

    num_tasks = 1 
    while True:
        with open(output_file_name,"a") as f:
            s = '%s Task : %d\n URL:%s\n Method:%s' % (datetime.datetime.now().strftime(("%Y-%m-%d %H:%M:%S")), num_tasks, url, sys.argv[3])
            print s
            print cmd
            f.writelines(s)
        try:
            p = Popen(cmd, shell=True)
            p.communicate()
            num_tasks += 1
            with open(output_file_name,"a") as f:
                f.write('\n')
        except KeyboardInterrupt:
            input = raw_input('\n\nTerminate the subprocess and exit?(y to exit, n to restart subprocess):')
            if input == 'y':
                p.terminate()
                os.system('dos2unix -c mac %s' % output_file_name)
                os._exit(-1)

