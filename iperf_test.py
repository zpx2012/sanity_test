import subprocess, os, time, datetime, socket
from os.path import expanduser

results_dir_abs_path = expanduser("~") + "/sanity_test_results/"
# os.system("mkdir %s" % results_dir_abs_path)
output_file_name = results_dir_abs_path + "iperf3_" + socket.gethostname() + "_" + datetime.datetime.now().strftime("%m%d%H%M")+".txt"

while True:
    p = subprocess.Popen('iperf3 -c 169.235.31.181 -p 80 -f kbits -b 1M -t 5 -4RV --logfile {}'.format(output_file_name),stderr=subprocess.PIPE,stdout=subprocess.PIPE, shell=True)
    stdoutdata, stderrdata = p.communicate()
    print stdoutdata

    # lines = stdoutdata.split('\n')
    # for line in lines:
    #     check_str = line[39:48]
    #     if check_str == 'Kbits/sec':
    #         speed = float(line[32:39])
    #         localtime = time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))
    #         with open(output_file_name,"a") as f:
    #             f.writelines(localtime + "\t  %10.1fk/s \n" %(speed))
    #             print(localtime + "\t  %10.1fk/s \n" %(speed))
    time.sleep(20)

