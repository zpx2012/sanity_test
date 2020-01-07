#!/bin/bash

stime=$(date -u +%m%d%H%M)

cat $(hostname).csv | while IFS=',' read ip hn; do
    screen -dmS td bash ~/sanity_test/ip_scan/tcpdump_whole.sh $ip 5000 $hn s
    while true;do
        aout=~/sanity_test/rs/hping3_ptest_$(hostname)_${hn}_${dp}_${sp}_ack_stdout_${stime}.txt
        aerr=~/sanity_test/rs/hping3_ptest_$(hostname)_${hn}_${dp}_${sp}_ack_stderr_${stime}.txt
        dout=~/sanity_test/rs/hping3_ptest_$(hostname)_${hn}_${dp}_${sp}_data_stdout_${stime}.txt
        derr=~/sanity_test/rs/hping3_ptest_$(hostname)_${hn}_${dp}_${sp}_data_stderr_${stime}.txt
        date -u +"%Y-%m-%d %H:%M:%S %Z" | tee -a $dout $derr
        sudo hping3 -PA -d 1400 -i 0.1 -c 60 -s 80 -k -p 5000 $ip >> $dout 2>> $derr
        date -u +"%Y-%m-%d %H:%M:%S %Z" | tee -a $aout $aerr
        sudo hping3 -A -i 0.1 -c 60 -s 80 -k -p 5000 $ip >> $aout 2>> $aerr
    done
done