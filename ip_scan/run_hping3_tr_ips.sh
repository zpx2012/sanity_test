#!/bin/bash
n=333456
cat $1 | while IFS=' ' read ip port; do
    i=7
    while (( i < 10 ));do
        screen -dmS hping3_tr_${ip}_$n bash ~/sanity_test/ip_scan/hping3_tr.sh $ip $port $n SA u500000 120 $i
        ((i++))
    done
    ((n++))
done