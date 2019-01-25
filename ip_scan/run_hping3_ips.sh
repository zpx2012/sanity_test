#!/bin/bash
n=`shuf -i 1024-65535 -n 1`
trfile=~/sanity_test_results/tr_redo0125_$(hostname)_$(date -u +"%m%d%H%M").txt
cat $1 | while IFS=' ' read ip port; do
    screen -dmS run_$ip bash ~/sanity_test/ip_scan/perip_hping3.sh $ip $port $n $trfile
    ((n++))
done