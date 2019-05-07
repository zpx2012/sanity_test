#!/bin/bash
cd ~/sanity_test/closed-port
mtr=~/sanity_test/mtr-modified/mtr
for day_i in 0 1 2 3 4; do
    screen -dmS td bash tcpdump_all.sh
    for p_i in 0 1 2 3;do
        screen -dmS sched_$p_i python sched.py 0505/$(hostname)_d${day_i}_p$(p_i).csv 
    done
    sleep 86400
    bash ~/sanity_test/ks.sh mtr
    bash ~/sanity_test/ks.sh td
done