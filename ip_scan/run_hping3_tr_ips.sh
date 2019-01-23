#!/bin/bash
n=33456
cat $1 | while IFS=' ' read ip port f m; do
    i=$f
    while (( i < $m ));do
        screen -dmS hping3_tr_${ip}_$i bash ~/sanity_test/ip_scan/hping3_tr.sh $ip $port $n SA u500000 120 $i
        ((i++))
    done
    ((n++))
done