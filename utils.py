import os,sys,time,datetime,socket,threading,csv,shlex
from subprocess import Popen, PIPE, TimeoutExpired,call
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
        call(shlex.split(cmd),timeout=sec)

    except TimeoutExpired:
        print('\n\n--------------\ncatch TimeoutExpired. Killed\n-------------\n')

    except KeyboardInterrupt:
        inp = input('\n\nTerminate the subprocess and exit?(y to exit, n to restart subprocess):')
        if inp == 'y':
            os._exit(-1)


# def run_cmd_wtimer(cmd,sec):
#     try:
#         p = Popen('exec ' + cmd, shell=True,universal_newlines=True)
#         p.communicate(timeout=sec)
    
#     except TimeoutExpired:
#         print('\n\ncatch TimeoutExpired:%d' % p.pid)
#         p.kill()
#         while not p.poll():
#             print('p not killed')
#         print('%d killed' % p.pid)

#     except KeyboardInterrupt:
#         inp = input('\n\nTerminate the subprocess and exit?(y to exit, n to restart subprocess):')
#         if inp == 'y':
#             p.terminate()
#             os._exit(-1)