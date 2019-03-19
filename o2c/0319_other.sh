#!/bin/bash
cd ~/sanity_test/o2c
mtr=~/sanity_test/mtr-insertion/mtr
for day_i in 0 1 2 3; do
    dfile=$(hostname)_$(date -u +"%m%d").csv
    cat $dfile | while IFS=' ' read ip hn dp sp; do
        screen -dmS td_$hn bash ~/sanity_test/ip_scan/tcpdump_whole.sh $ip $sp $hn s
    done
    screen -dmS mtr bash ~/sanity_test/con2con/0304_mtr_poll_check.sh $dfile $mtr
    sleep 86400
    bash ~/sanity_test/ks.sh mtr
    bash ~/sanity_test/ks.sh td
done