import os,sys,time,datetime,socket,threading,csv
from subprocess import Popen, PIPE
from time import sleep
from os.path import expanduser
from utils import run_cmd_wtimer
from random import randint

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage:\n\tpython curl_downloader.py [URL] [IP] [Tool] [Site]\n\nOptions:\n\tTool: ss, vpn, ssh, from which server\n\tSite: download source, 163 or mit or so")
        sys.exit(-1)

    decorator = '\n********************************\n'
    print (decorator + 'scp tester 0.0.1\nCtrl-C to terminate the program' + decorator + '\n')
    
    out_dir = expanduser('~/sanity_test_results/')
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)
    output_filename_list = []

    infile_name = sys.argv[1]
    # base_cmd = 'script -q /dev/stdout -c "scp -oStrictHostKeyChecking=no -l 4000 -i %s ~/my.pcap %s@%s:/dev/null" | tee -a %s'
    base_cmd = 'script -aqf -c "scp -oStrictHostKeyChecking=no -l 4000 -i %s ~/my.pcap %s@%s:/dev/null" %s'

    if not os.path.isfile(infile_name):
        print('File does not exist.')
    else:
        with open(infile_name,'r') as f:         
            domain_ip_list = list(csv.reader(f))
        for i,line in enumerate(domain_ip_list):
            output_filename_list.append(out_dir + '_'.join(['scp_upload',socket.gethostname()+'2'+line[3],datetime.datetime.utcnow().strftime('%m%d%H%Mutc')]) +'.txt')
        num_tasks = 1 
        while True:
            for i,line in enumerate(domain_ip_list):
                cmd = base_cmd % (line[0],line[1],line[2],output_filename_list[i])
                print(cmd)
                with open(output_filename_list[i],'a') as f:
                    f.writelines('\n\n%s Task : %d\n %s' % (datetime.datetime.utcnow().strftime('%Y-%m-%d %H:%M:%S +0000'), num_tasks, cmd))
                run_cmd_wtimer(cmd,10)
                #visit_cn_websites(8)
                num_tasks += 1