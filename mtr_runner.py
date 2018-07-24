import os,sys,time,datetime,socket,shlex
from subprocess import Popen, PIPE
from time import sleep
from os.path import expanduser

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage:\n\tpython mtr_runner.py [URL] [URL Nickname for Output]")
        sys.exit(-1)

    output_file_name = expanduser("~") + "/results/mtr_" + socket.gethostname() + "_" + sys.argv[2] + "_" + datetime.datetime.now().strftime("%m%d%H%M")+".txt"

    decorator = '\n********************************\n'
    print decorator + 'Mtr Runner 1.0.0\nCtrl-C to terminate the program' + decorator + '\n'

    cmd = 'mtr --tcp -c 1 --report ' + sys.argv[1]

    num_tasks = 1 
    while True:
        # with open(output_file_name,"a") as f:
        #     s = '%s Task : %d\n URL:%s\n Method:%s' % (datetime.datetime.now().strftime(("%Y-%m-%d %H:%M:%S")), num_tasks, sys.argv[1], sys.argv[3])
        #     print s
        #     f.writelines(s)
        try:
            p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE)
            sout, serr = p.communicate()
            print 'stdout:\n %s' % sout
            print 'sterr:\n %s ' % serr
            num_tasks += 1
            sleep(10)
        except KeyboardInterrupt:
            input = raw_input('\n\nTerminate the subprocess and exit?(y to exit, n to restart subprocess):')
            if input == 'y':
                p.terminate()
                os._exit(-1)

