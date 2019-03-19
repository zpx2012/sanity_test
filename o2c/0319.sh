#!/bin/bash
cd ~/sanity_test/o2c
day_i=0
n=0
node_i=$1
mtr=~/sanity_test/mtr-insertion/mtr
tested=tested_$(date -u +"%m%d%H%M")
for day_i in 0 1 2 3; do
    tf=cur_$(date -u +"%m%d%H%M")
    cat day${day_i}_node${node_i}.csv | while IFS=',' read ip hn dp sp; do
        screen -dmS td_$hn bash ~/sanity_test/ip_scan/tcpdump_whole.sh $ip $dp $hn
        screen -dmS curl_$hn python ~/sanity_test/curl_downloader.py "http://$ip/my.pcap" $ip $hn 0 1000k 0 $((sp+1))
    done
    screen -dmS mtr bash ~/sanity_test/con2con/0304_mtr_poll_check.sh day${day_i}_node${node_i}.csv $mtr
    sleep 86400
    bash ~/sanity_test/ks.sh mtr
    bash ~/sanity_test/ks.sh td
    bash ~/sanity_test/ks.sh curl
    date -u +"%m%d%H%M" >> $tested
    cat $tf >> $tested
    rm $tf
done