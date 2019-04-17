#!/bin/bash
cd ~/sanity_test/prot
day_i=0
n=0
mtr=~/sanity_test/mtr-modified/mtr
for day in 0 1:
    cat data/$(hostname)_${day}.csv | while IFS=',' read ip hn http_sp https_sp ss_lp ss_sp iperf_dp iperf_sp; do
        screen -dmS td_$hn bash tcpdump_iponly.sh $ip $hn
        screen -dmS poller_$hn poller.sh ip hn http_sp https_sp ss_lp ss_sp iperf_dp iperf_sp
        screen -dmS mtr bash ~/sanity_test/con2con/0304_mtr_poll.sh data/mtr_$(hostname)_${day}.csv $mtr
    done
    sleep 86400
    bash ~/sanity_test/ks.sh mtr
    bash ~/sanity_test/ks.sh td
    bash ~/sanity_test/ks.sh poller
done