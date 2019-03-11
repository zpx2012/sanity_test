#!/bin/bash
ip=$1
dp=$2
sp=$3
out=~/sanity_test/rs/hping3_closed_${ip}_${dp}_${sp}_${4}_stdout_$(hostname)_$(date -u +"%m%d%H%M").txt
err=~/sanity_test/rs/hping3_closed_${ip}_${dp}_${sp}_${4}_stderr_$(hostname)_$(date -u +"%m%d%H%M").txt
tfile=~/sanity_test/rs/ptraceroute_${ip}_${dp}_${sp}_$(hostname)_$(date -u +%m%d%H%M).txt
while true;do
date -u +"%Y-%m-%d %H:%M:%S %Z" | tee -a $out $err $tfile
sudo hping3 -$4 -i $5 -c $6 -s $sp -k -p $dp $ip >> $out 2>> $err 
sudo paris-traceroute -Q -s $sp -d $dp -p tcp -f 4 -m 25 $ip >> $tfile
done