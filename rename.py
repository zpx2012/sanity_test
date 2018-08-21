import os,sys,socket

if __name__ == '__main__':
    
    os.system('move ~/iperf3.log ~/sanity_test_results/iperf3_%s.log' % socket.gethostname())