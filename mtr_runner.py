import os,sys,time,datetime,socket,shlex,csv
from subprocess import Popen, PIPE
from time import sleep
from os.path import expanduser


def run_cmd_log(cmd,outfile):
    try:
        p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE)
        sout, serr = p.communicate()
        print 'stout:\n %s\n sterr:\n %s\n' % (sout, serr)
        if not sout:
            print '#######\n empty stdout'
        with open(outfile,"a") as f:
            f.writelines(cmd+'\n'+sout+'\n'+serr) 

    except KeyboardInterrupt:
        input = raw_input('\n\nTerminate the subprocess and exit?(y to exit, n to restart subprocess):')
        if input == 'y':
            p.terminate()
            os._exit(-1)


if __name__ == '__main__':
    if len(sys.argv) < 5:
        print("Usage:\n\tpython mtr_runner.py [new/old] [firstTTL] [cycle_num] [in.csv]")
        # print("Usage:\n\tpython mtr_runner.py [URL] [URL Nickname for Output]")
        sys.exit(-1)

    decorator = '\n********************************\n'
    print decorator + 'Mtr Runner 1.2.0\nCtrl-C to terminate the program' + decorator + '\n'

    out_dir = expanduser('~/sanity_test_results/')
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)
    output_filename_dict = {} 

    version_dict = {'old':'mtr','new':'~/mtr-modified/mtr'}
    base_cmd = 'sudo {} -zwnr4 -f {} --report-cycles {} --port %s --tcp %s'.format(version_dict[sys.argv[1]],sys.argv[2],sys.argv[3])

    infile_name = sys.argv[4]
    
    if not os.path.isfile(infile_name):
        print 'File does not exist.'
    else:
        with open(infile_name,'rb') as f:         
            domain_ip_list = list(csv.reader(f))
        for line in domain_ip_list:
            output_filename_dict[line[0]] = out_dir + "mtr_" + sys.argv[1] + '_' + socket.gethostname() + "2" + line[0] + "_" + sys.argv[3] +'_'+ datetime.datetime.now().strftime("%m%d%H%M")+".txt"
        num_tasks = 1 
        while True:
                for line in domain_ip_list:
                    cmd = base_cmd % (line[2],line[1])
                    print cmd
                    run_cmd_log(cmd,output_filename_dict[line[0]])
                    num_tasks += 1


