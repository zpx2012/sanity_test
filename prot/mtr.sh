#!/bin/bash
ip=$1
hn=$2
dp=$3
sp=$4
cnt=$5
mtr=$6
startime=$(date -u +"%m%d%H%M")utc
while true
do 
sudo $mtr -zwnr4T -P $dp -L $sp -c $cnt $ip 2>&1 | tee -a ~/sanity_test/rs/mtrins_$(hostname)_${hn}_tcp_1_60_$startime.txt
done