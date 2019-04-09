#!/bin/bash
cd ~/sanity_test/closed-port
mtr=~/sanity_test/mtr-modified/mtr
for day_i in 0 1 2 3 4; do
    screen -dmS mtr4 bash 0406_mtr_poll.sh $(hostname)_day${day_i}_p2.csv $mtr
done