import os,sys,time,datetime,socket,threading,csv,shlex,signal
from subprocess import Popen, PIPE, TimeoutExpired,call,check_output
from time import sleep
from os.path import expanduser

def run_cmd_log(cmd,outfile):
    sout, serr = run_cmd(cmd)
    with open(outfile,'a') as f:
        f.writelines(cmd+'\n'+sout+'\n'+serr) 

def run_cmd_shell(cmd):
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
        cmds = shlex.split(cmd)
        print(cmds)
        call(cmds,timeout=sec)

    except TimeoutExpired:
        print('\n\n--------------\ncatch TimeoutExpired. Killed\n-------------\n')
        # call('ps -ef | grep {0}'.format(cmds[0]),shell=True)

    except KeyboardInterrupt:
        inp = input('\n\nTerminate the subprocess and exit?(y to exit, n to restart subprocess):')
        if inp == 'y':
            os._exit(-1)

def run_cmd_wtimer_slient(cmd,sec):

    try:
        cmds = shlex.split(cmd)
        print(cmds)
        call(cmds,timeout=sec,stdout=PIPE,stderr=PIPE)

    except TimeoutExpired:
        print('\n\n--------------\ncatch TimeoutExpired. Killed\n-------------\n')
        # call('ps -ef | grep {0}'.format(cmds[0]),shell=True)

    except KeyboardInterrupt:
        inp = input('\n\nTerminate the subprocess and exit?(y to exit, n to restart subprocess):')
        if inp == 'y':
            os._exit(-1)

def run_cmd_shell_wtimer(cmd,sec):

    try:
        p = Popen('exec '+cmd,shell=True, preexec_fn=os.setpgrp) 
        p.communicate(timeout=sec)

    except TimeoutExpired:
        print('\n\n--------------\ncatch TimeoutExpired. Killed\n-------------\n')
        os.killpg(os.getpgid(p.pid), signal.SIGKILL)
        cmds = shlex.split(cmd)
        os.system('set -v;sudo killall -9 %s' % cmds[1] if cmds[0] == 'sudo' else cmds[0])
        print('%d killed' % p.pid)

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