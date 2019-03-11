#!/bin/bash
cd ~/sanity_test/con2con
day_i=0
n=0
mtr=~/sanity_test/mtr-modified/mtr
tested=~/sanity_test/con2con/tested_$(date -u +"%m%d%H%M")
tf=no
cat ~/sanity_test/con2con/$(hostname).csv | while IFS=',' read ip hn sp nm io; do
    tf=~/sanity_test/con2con/cur_$(date -u +"%m%d%H%M")
    echo $ip $hn 80 $sp >> $tf
    screen -dmS td_$hn bash ~/sanity_test/ip_scan/tcpdump_whole.sh $ip 80 $hn
    if (( n % 2 == 0 ));then #out bound
        screen -dmS curl_$hn python ~/sanity_test/curl_downloader.py "http://$ip/my.pcap" $ip $hn 0 1000k 0 $((sp+1))
    fi
    let day_i=day_i+1
    let n=n+1            
    if (( day_i == 4 )) || ((n == 18));then
        screen -dmS mtr bash ~/sanity_test/con2con/0304_mtr_poll_check.sh $tf $mtr
        sleep 86400
        bash ~/sanity_test/ks.sh mtr
        bash ~/sanity_test/ks.sh td
        bash ~/sanity_test/ks.sh curl
        date -u +"%m%d%H%M" >> $tested
        cat $tf >> $tested
        rm $tf
        let day_i=0
    fi
done
