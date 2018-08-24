import os,sys,time,datetime,socket,shlex
from subprocess import Popen, PIPE
from time import sleep
from os.path import expanduser

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage:\n\tpython mtr_runner.py [URL] [URL Nickname for Output]")
        sys.exit(-1)

    out_dir = expanduser('~/sanity_test_results/')
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)
    output_file_name = out_dir + "mtr_" + socket.gethostname() + "_" + sys.argv[2] + "_" + datetime.datetime.now().strftime("%m%d%H%M")+".txt"

    decorator = '\n********************************\n'
    print decorator + 'Mtr Runner 1.0.6\nCtrl-C to terminate the program' + decorator + '\n'

    cmd = 'mtr -zwnr4 --port 80 --tcp ' + sys.argv[1]

    num_tasks = 1 
    while True:
        try:
            p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE)
            sout, serr = p.communicate()
            print 'stout %s\n sterr: %s' % (sout, serr)
            if not sout:
                print '#######\n empty stdout'
            with open(output_file_name,"a") as f:
                f.writelines(sout)            
            num_tasks += 1
            sleep(10)
        except KeyboardInterrupt:
            input = raw_input('\n\nTerminate the subprocess and exit?(y to exit, n to restart subprocess):')
            if input == 'y':
                p.terminate()
                os._exit(-1)

