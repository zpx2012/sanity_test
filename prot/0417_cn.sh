#!/bin/bash
cd ~/sanity_test/prot
mtr=~/sanity_test/mtr-modified/mtr
for day in 0 1;do
    cat data/$(hostname)_day${day}.csv | while IFS=' ' read ip hn http_sp https_sp ss_lp ss_sp iperf_dp iperf_sp; do
        screen -dmS td_$hn bash tcpdump_iponly.sh $ip $hn
        screen -dmS poller_$hn poller.sh ip hn http_sp https_sp ss_lp ss_sp iperf_dp iperf_sp
        screen -dmS mtr bash ~/sanity_test/con2con/0304_mtr_poll.sh data/mtr_$(hostname)_${hn}.csv $mtr
    done
    sleep 86400
    bash ~/sanity_test/ks.sh mtr
    bash ~/sanity_test/ks.sh td
    bash ~/sanity_test/ks.sh pollers
done