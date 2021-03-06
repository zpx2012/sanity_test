#!/bin/bash
ip=$1
dp=$2
sp=$3
speed=$4
cnt=$5
out=~/sanity_test/rs/hping3_closed_${ip}_${dp}_${sp}_data_stdout_$(hostname)_$(date -u +"%m%d%H%M").txt
err=~/sanity_test/rs/hping3_closed_${ip}_${dp}_${sp}_data_stderr_$(hostname)_$(date -u +"%m%d%H%M").txt
while true;do
	date -u +"%Y-%m-%d %H:%M:%S %Z" | tee -a $out $err
	sudo hping3 -PA -d 1400 -i $speed -c $cnt -s $sp -k -p $dp $ip >> $out 2>> $err
done