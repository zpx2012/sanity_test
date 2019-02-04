import os,sys,time,datetime,socket,shlex,csv,random
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

def run_cmd(cmd):
    try:
        p = Popen(cmd, shell=True, stdout=PIPE, stderr=PIPE)
        sout, serr = p.communicate()
        print 'stout:\n %s\nsterr:\n %s\n' % (sout, serr)
        if not sout:
            print '#######\n empty stdout' 
        return sout,serr

    except KeyboardInterrupt:
        input = raw_input('\n\nTerminate the subprocess and exit?(y to exit, n to restart subprocess):')
        if input == 'y':
            p.terminate()
            os._exit(-1)

def via_4134(line):
    sout,serr = run_cmd('sudo traceroute -A -p %s -T -f 4 -m 25 %s'%(line[5],line[8]))
    rt = sout + serr
    if '202.97' in rt or 'AS4134' in rt:
        return True
    return False

if __name__ == '__main__':
    if len(sys.argv) < 1:
        print("Usage:\n\tpython mtr_runner.py [new/old] [firstTTL] [cycle_num] [in.csv]")
        # print("Usage:\n\tpython mtr_runner.py [URL] [URL Nickname for Output]")
        sys.exit(-1)

    decorator = '\n********************************\n'
    print decorator + 'Mtr Runner 1.4.0\nCtrl-C to terminate the program' + decorator + '\n'

    out_dir = expanduser('~/sanity_test_results/')
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)
    output_filename_list = []

    version_dict = {'old':'mtr','1':'~/mtr-modified-1.0/mtr','2':'~/mtr-modified-2.0/mtr'}
    base_cmd = 'sudo %s -zwnre4%s -i %s -c %s -f %s %s %s 2>&1 | tee -a %s'#.format(version_dict[sys.argv[1]],sys.argv[2],sys.argv[3])

    infile_name = sys.argv[1]
    
    if not os.path.isfile(infile_name):
        print 'File does not exist.'
    else:
        domain_ip_list = []
        with open(infile_name,'rb') as f:         
            domain_ip_list = list(csv.reader(f))
        dlen = len(domain_ip_list)
        if dlen == 0:
            sys.exit(-1)
        for i,line in enumerate(domain_ip_list):
            if via_4134(line):
                print('via_4134 return true')
                output_filename_list.append(out_dir + "mtr_" + line[0] + '_' + socket.gethostname() + "2" + line[6] + '_' + line[7]+'_'+line[2] + '_' + line[3] +'_'+ datetime.datetime.now().strftime("%m%d%H%M")+".txt")
            else:
                domain_ip_list.pop(i)
        num_tasks = 1 
        r = random.randint(0,dlen-1)
        while True:
            for i in range(dlen):
                line = domain_ip_list[(r+i)%dlen]
                port_str = ''
                if line[8] != '0':
                    port_str = '--port %s' % line[8]
                cmd = base_cmd % (version_dict[line[0]],line[1],line[2],line[3],line[4],port_str,line[5],output_filename_list[i])
                print cmd
                os.system('echo %s >> %s'%(cmd,output_filename_list[i]))
                run_cmd(cmd)
                # run_cmd_log(cmd,output_filename_dict[line[0]])
                num_tasks += 1


