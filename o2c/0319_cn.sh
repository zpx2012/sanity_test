#!/bin/bash
if [ -z "$1" ]
  then
    echo "No argument supplied"
    exit 1
fi
cd ~/sanity_test/o2c
node_i=$1
today=$2
mtr=~/sanity_test/mtr-insertion/mtr
for ((day_i=today; day_i<4; day_i++)); do
    dfile=node${node_i}_day${day_i}.csv
    cat $dfile | while IFS=' ' read ip hn dp sp; do
        screen -dmS td_$hn bash ~/sanity_test/ip_scan/tcpdump_whole.sh $ip $dp $hn c
        screen -dmS curl_$hn python ~/sanity_test/curl_downloader.py "http://$ip/my.pcap" $ip $hn 0 700k 0 $sp
    done
    screen -dmS mtr bash ~/sanity_test/con2con/0304_mtr_poll_check.sh $dfile $mtr
    sleep 86400
    bash ~/sanity_test/ks.sh mtr
    bash ~/sanity_test/ks.sh td
    bash ~/sanity_test/ks.sh curl
done