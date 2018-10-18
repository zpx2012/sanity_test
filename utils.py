import os,sys,time,datetime,socket,urlparse,threading,csv
from subprocess import Popen, PIPE
from time import sleep
from os.path import expanduser

def run_cmd_log(cmd,outfile):
    sout, serr = run_cmd(cmd)
    with open(outfile,'a') as f:
        f.writelines(cmd+'\n'+sout+'\n'+serr) 

def run_cmd(cmd):
    try:
        p = Popen(cmd, shell=True)
        p.communicate()
        # print 'stout:\n %s\nsterr:\n %s\n' % (sout, serr)
        # if not sout:
        #     print '#######\n empty stdout' 
        # return sout, serr
    except KeyboardInterrupt:
        input = raw_input('\n\nTerminate the subprocess and exit?(y to exit, n to restart subprocess):')
        if input == 'y':
            p.terminate()
            os._exit(-1)

def run_cmd_wtimer(cmd,sec):
    try:
        p = Popen(cmd, shell=True)
        p.communicate(timeout=sec)
    
    except TimeoutExpired:
        p.kill()

    except KeyboardInterrupt:
        input = raw_input('\n\nTerminate the subprocess and exit?(y to exit, n to restart subprocess):')
        if input == 'y':
            p.terminate()
            os._exit(-1)