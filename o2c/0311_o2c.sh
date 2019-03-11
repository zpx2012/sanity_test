#!/bin/bash
cd ~/sanity_test/con2con
day_i=0
n=0
mtr=~/mtr-modified/mtr
tested=~/sanity_test/con2con/tested_$(date -u +"%m%d%H%M")
tf=no
while true; do
    cat ~/sanity_test/con2con/$(hostname)_$(date -u +"%m%d").csv | while IFS=',' read ip hn sp; do
        tf=~/sanity_test/con2con/cur_$(date -u +"%m%d%H%M")
        echo $ip $hn 80 $sp >> $tf
        screen -dmS td_$hn bash ~/sanity_test/ip_scan/tcpdump_whole.sh $ip 80 $hn
        screen -dmS curl_$hn python ~/sanity_test/curl_downloader.py "http://$ip/my.pcap" $ip $hn 0 1000k 0 $((sp+1))
    done
    screen -dmS mtr bash ~/sanity_test/con2con/0304_mtr_poll_check.sh $tf $mtr
    sleep 86400
    bash ~/sanity_test/ks.sh mtr
    bash ~/sanity_test/ks.sh td
    bash ~/sanity_test/ks.sh curl
    date -u +"%m%d%H%M" >> $tested
    cat $tf >> $tested
    rm $tf
done