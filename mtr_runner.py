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
    if len(sys.argv) < 1:
        print("Usage:\n\tpython mtr_runner.py [new/old] [firstTTL] [cycle_num] [in.csv]")
        # print("Usage:\n\tpython mtr_runner.py [URL] [URL Nickname for Output]")
        sys.exit(-1)

    decorator = '\n********************************\n'
    print decorator + 'Mtr Runner 1.3.0\nCtrl-C to terminate the program' + decorator + '\n'

    out_dir = expanduser('~/sanity_test_results/')
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)
    output_filename_list = []

    version_dict = {'old':'mtr','new':'~/mtr-modified/mtr'}
    base_cmd = 'sudo %s -zwnr4%s -i %s -c %s -f %s --port 80 %s 2>&1 | tee -a %s'#.format(version_dict[sys.argv[1]],sys.argv[2],sys.argv[3])

    infile_name = sys.argv[1]
    
    if not os.path.isfile(infile_name):
        print 'File does not exist.'
    else:
        with open(infile_name,'rb') as f:         
            domain_ip_list = list(csv.reader(f))
        for i,line in enumerate(domain_ip_list):
            output_filename_list.append(out_dir + "mtr_" + line[0] + '_' + socket.gethostname() + "2" + line[6] + '_' + line[7]+'_'+line[2] + '_' + line[3] +'_'+ datetime.datetime.now().strftime("%m%d%H%M")+".txt")
        num_tasks = 1 
        while true:
                for i,line in enumerate(domain_ip_list):
                    cmd = base_cmd % (version_dict[line[0]],line[1],line[2],line[3],line[4],line[5],output_filename_list[i])
                    print cmd
                    os.system('echo %s >> %s'%(cmd,output_filename_list[i]))
                    os.system(cmd)
                    # run_cmd_log(cmd,output_filename_dict[line[0]])
                    num_tasks += 1


