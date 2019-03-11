import os,sys,socket

if __name__ == '__main__':
    
    os.system('mv ~/iperf3.log ~/sanity_test/rs/iperf3_%s.log' % socket.gethostname())