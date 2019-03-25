#!/bin/bash
cd ~/sanity_test/o2c
mtr=~/sanity_test/mtr-insertion/mtr
for day_i in 0 1 2 3; do
    dfile=$(hostname)_$(date -u +"%m%d").csv
    screen -dmS td bash ~/sanity_test/ip_scan/tcpdump_server.sh $sp
    screen -dmS mtr bash ~/sanity_test/con2con/0304_mtr_poll_check.sh $dfile $mtr
    sleep 86400
    bash ~/sanity_test/ks.sh mtr
    bash ~/sanity_test/ks.sh td
done