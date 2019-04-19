#!/bin/bash
ip=$1
hn=$2
dp=$3
sp=$4
cnt=$5
mtr=$6
prot=$7
startime=$(date -u +"%m%d%H%M")utc
while true
do 
sudo $mtr -zwnr4T -P $dp -L $sp -c $cnt $ip | tee -a ~/sanity_test/rs/mtrins_$(hostname)_${hn}_${prot}_${dp}_${sp}_tcp_1_${cnt}_$startime.txt
done