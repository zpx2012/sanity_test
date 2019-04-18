#!/bin/bash
cd ~/sanity_test/prot
mtr=~/sanity_test/mtr-insertion/mtr
for day in 0 1;do
    screen -dmS td bash tcpdump_portonly.sh
    cat data/$(hostname)_day${day}.csv | while IFS=' ' read ip hn ; do
        screen -dmS mtr bash ~/sanity_test/con2con/0319_mtr_poll.sh data/mtr_$(hostname)_${hn}.csv $mtr
    done
    sleep 86400
    bash ~/sanity_test/ks.sh mtr
    bash ~/sanity_test/ks.sh td
done