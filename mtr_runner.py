import os,sys,time,datetime,socket,shlex,csv
from subprocess import Popen, PIPE
from time import sleep
from os.path import expanduser

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage:\n\tpython mtr_runner.py [in.csv]")
        # print("Usage:\n\tpython mtr_runner.py [URL] [URL Nickname for Output]")
        sys.exit(-1)

    out_dir = expanduser('~/sanity_test_results/')
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)
    output_filename_dict = {} 

    decorator = '\n********************************\n'
    print decorator + 'Mtr Runner 1.1.0\nCtrl-C to terminate the program' + decorator + '\n'

    base_cmd = 'mtr -zwnr4 --report-cycles 2 --port 80 --tcp %s'

    infile_name = sys.argv[1]
    
    if not os.path.isfile(infile_name):
        print 'File does not exist.'
    else:
        with open(infile_name,'rb') as f:         
            domain_ip_list = list(csv.reader(f))
        for line in domain_ip_list:
            output_filename_dict[line[0]] = out_dir + "mtr_" + socket.gethostname() + "_" + line[0] + "_" + datetime.datetime.now().strftime("%m%d%H%M")+".txt"
        num_tasks = 1 
        while True:
            try:
                for line in domain_ip_list:
                    cmd = base_cmd % line[1]
                    print cmd
                    p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE)
                    sout, serr = p.communicate()
                    print 'stout %s\n sterr: %s' % (sout, serr)
                    if not sout:
                        print '#######\n empty stdout'
                    with open(output_filename_dict[line[0]],"a") as f:
                        f.writelines(sout)            
                    num_tasks += 1
            except KeyboardInterrupt:
                input = raw_input('\n\nTerminate the subprocess and exit?(y to exit, n to restart subprocess):')
                if input == 'y':
                    p.terminate()
                    os._exit(-1)

