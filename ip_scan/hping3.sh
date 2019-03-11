#!/bin/bash
ip=$1
dp=$2
sp=$3
out=~/sanity_test/rs/hping3_closed_${ip}_${dp}_${sp}_${4}_stdout_$(hostname)_$(date -u +"%m%d%H%M").txt
err=~/sanity_test/rs/hping3_closed_${ip}_${dp}_${sp}_${4}_stderr_$(hostname)_$(date -u +"%m%d%H%M").txt
while true;do
date -u +"%Y-%m-%d %H:%M:%S %Z" | tee -a $out $err
hping3 -$4 -i $5 -c $6 -s $sp -k -p $dp $ip >> $out 2>> $err
done