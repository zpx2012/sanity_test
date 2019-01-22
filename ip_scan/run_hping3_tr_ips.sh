#!/bin/bash
n=7
cat $1 | while IFS=' ' read ip port; do
    while (( n < 10 ));do
        screen -dmS hping3_tr_$ip_$n bash ~/sanity_test/ip_scan/hping3_tr.sh $ip $port $n SA u500000 120 $n
        ((n++))
    done
    n=7
done