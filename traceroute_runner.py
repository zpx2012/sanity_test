import os,sys,time,datetime,socket,urlparse,threading,csv
from subprocess import Popen, PIPE
from time import sleep
from os.path import expanduser

if __name__ == '__main__':
    
    infile_name = sys.argv[1]
    
    if not os.path.isfile(infile_name):
        print 'File does not exist.'
    else:
        with open(infile_name,'rb') as f: 
            for line in list(csv.reader(f)):
                output_file_name = '~/sanity_test/rs/tr_' + socket.gethostname() + "_" + line[0] + "_" + datetime.datetime.now().strftime("%m%d%H%M")+".txt"
                os.system('traceroute -A {} > {}'.format(line[1],output_file_name))