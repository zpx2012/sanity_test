#!/bin/bash
cd ~/sanity_test/closed-port
mtr=~/sanity_test/mtr-modified/mtr
for day_i in 0 1 2 3 4; do
    screen -dmS td bash tcpdump_all.sh
    screen -dmS mtr0 bash 0406_mtr_poll.sh data/$(hostname)_day${day_i}_p0.csv $mtr
    screen -dmS mtr1 bash 0406_mtr_poll.sh data/$(hostname)_day${day_i}_p1.csv $mtr
    screen -dmS mtr1 bash 0406_mtr_poll.sh data/$(hostname)_day${day_i}_p2.csv $mtr
    screen -dmS mtr2 bash 0406_mtr_poll.sh data/$(hostname)_day$((day_i+5))_p0.csv $mtr
    screen -dmS mtr3 bash 0406_mtr_poll.sh data/$(hostname)_day$((day_i+5))_p1.csv $mtr
    screen -dmS mtr3 bash 0406_mtr_poll.sh data/$(hostname)_day$((day_i+5))_p2.csv $mtr
    sleep 86400
    bash ~/sanity_test/ks.sh mtr
    bash ~/sanity_test/ks.sh td
done