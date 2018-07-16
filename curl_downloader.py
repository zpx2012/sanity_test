import os,sys,time,datetime,socket,shlex
from subprocess import Popen, PIPE
from time import sleep
from os.path import expanduser

if __name__ == '__main__':
    if len(sys.argv) < 5:
        print("Usage:\n\tpython curl_downloader.py [URL] [Mode] [Tool] [Site]\n\nOptions:\n\tMode:0:regular, 1:proxy mode\n\tTool: ss, vpn, ssh, from which server\n\tSite: download source, 163 or mit or so")
        sys.exit(-1)

    os.system("mkdir ~/results")
    output_file_name = expanduser("~") + "/results/" + socket.gethostname() + "_" + sys.argv[3] + "_" + sys.argv[4] + "_" + datetime.datetime.now().strftime("%m%d%H%M")+".txt"

    decorator = '********************************'
    print decorator + '\nWelcome to curl downloader\n\nCtrl-C to terminate the program\n' + decorator + '\n\n'

    
    if(sys.argv[2] == '0'):
        cmd = 'curl -o /dev/null --limit-rate 1000k %s 2>&1 | tee -a %s' % (sys.argv[1], output_file_name)
    else:
        cmd = 'curl -o /dev/null --limit-rate 1000k --socks localhost:1080 %s 2>&1 | tee -a %s' % (sys.argv[1], output_file_name)

    num_tasks = 1 
    while True:
        with open(output_file_name,"a") as f:
            s = '%s Task : %d\n' % (datetime.datetime.now().strftime(("%Y-%m-%d %H:%M:%S")), num_tasks)
            print s
            f.writelines(s)
        try:
            p = Popen(cmd, shell=True)
            p.communicate()
            num_tasks += 1
        except KeyboardInterrupt:
            input = raw_input('\n\nTerminate the subprocess and exit?(y to exit, n to restart subprocess):')
            if input == 'y':
                p.terminate()
                os._exit(-1)

