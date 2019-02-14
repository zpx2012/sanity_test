#!/bin/bash
ip1=$1
dp1=$2
sp1=$3
ip2=$7
dp2=$8
sp2=$9
out=~/sanity_test_results/hping3_closed_${ip1}_${dp1}_${sp1}_${4}_stdout_$(hostname)_$(date -u +"%m%d%H%M").txt
err=~/sanity_test_results/hping3_closed_${ip1}_${dp1}_${sp1}_${4}_stderr_$(hostname)_$(date -u +"%m%d%H%M").txt
tfile=~/sanity_test_results/ptraceroute_${ip2}_${dp2}_${sp2}_$(hostname)_$(date -u +%m%d%H%M).txt
while true;do
date -u +"%Y-%m-%d %H:%M:%S %Z" | tee -a $out $err $tfile
sudo hping3 -$4 -i $5 -c $6 -s $sp1 -k -p $dp1 $ip1 >> $out 2>> $err 
sudo paris-traceroute -Q -s $sp2 -d $dp2 -p tcp -f 4 -m 25 $ip2 >> $tfile
done