#!/bin/bash
n=33456
cat $1 | while IFS=' ' read ip port; do
    i=$2
    while (( i < $3 ));do
        screen -dmS hping3_tr_${ip}_$i bash ~/sanity_test/ip_scan/hping3_tr.sh $ip $port $n SA u500000 120 $i
        ((i++))
    done
    ((n++))
done