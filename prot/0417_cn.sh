#!/bin/bash
cd ~/sanity_test/prot
day_i=0
n=0
mtr=~/sanity_test/mtr-modified/mtr
tested=tested_$(date -u +"%m%d%H%M")
while true; do
    tf=cur_$(date -u +"%m%d%H%M")
    cat $(hostname)_$(date -u +"%m%d").csv | while IFS=' ' read ip hn sp; do
        echo $ip $hn 80 $sp >> $tf
        screen -dmS td_$hn bash ~/sanity_test/ip_scan/tcpdump_whole.sh $ip 80 $hn
        screen -dmS curl_$hn python ~/sanity_test/curl_downloader.py
        screen -dmS mtr bash ~/sanity_test/con2con/0304_mtr_poll.sh $tf $mtr
    done

    sleep 86400
    bash ~/sanity_test/ks.sh mtr
    bash ~/sanity_test/ks.sh td
    bash ~/sanity_test/ks.sh curl
    date -u +"%m%d%H%M" >> $tested
    cat $tf >> $tested
    rm $tf
done