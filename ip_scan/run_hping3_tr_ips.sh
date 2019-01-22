#!/bin/bash
n=7
cat $1 | while IFS=' ' read ip port; do
    if (( n < 10 ));then
        screen -dmS hping3_tr_$ip bash ~/sanity_test/ip_scan/hping3_tr.sh $ip $port $n SA u500000 120 $n
        ((n++))
    fi
done