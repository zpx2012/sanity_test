#!/bin/bash
cd ~/sanity_test/prot
mtr=~/sanity_test/mtr-insertion/mtr
for day in 0 1;do
    screen -dmS td bash tcpdump_portonly.sh
    cat data/$(hostname)_day${day}.csv | while IFS=' ' read ip hn http_sp https_sp ss_lp ss_sp iperf_dp iperf_sp; do
        screen -dmS mtr_${hn}_http bash mtr.sh $ip $hn $http_sp 80 60 $mtr
        screen -dmS mtr_${hn}_ssl bash mtr.sh $ip $hn $https_sp 443 60 $mtr
        screen -dmS mtr_${hn}_ss bash mtr.sh $ip $hn $ss_lp 8388 60 $mtr
        screen -dmS mtr_${hn}_iperf bash mtr.sh $ip $hn $iperf_sp $iperf_dp 60 $mtr 
    done
    sleep 86400
    bash ~/sanity_test/ks.sh mtr
    bash ~/sanity_test/ks.sh td
done