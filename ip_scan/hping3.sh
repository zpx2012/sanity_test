#!/bin/bash
ip=$1
dp=$2
sp=$3
out=hping3_closed_${ip}_${dp}_${sp}_stdout.txt
err=hping3_closed_${ip}_${dp}_${sp}_stderr.txt
while true;do
date -u +"%Y-%m-%d %H:%M:%S %Z" | tee -a $out $err
hping3 -SA -i u500000 -c 120 -s $sp -p $dp $ip >> $out 2>> $err
done