#!/bin/bash
ip=$1
dp=$2
sp=$3
type=$4
speed=$5
cnt=$6
out=~/sanity_test/rs/hping3_closed_${ip}_${dp}_${sp}_${4}_stdout_$(hostname)_$(date -u +"%m%d%H%M").txt
err=~/sanity_test/rs/hping3_closed_${ip}_${dp}_${sp}_${4}_stderr_$(hostname)_$(date -u +"%m%d%H%M").txt
date -u +"%Y-%m-%d %H:%M:%S %Z" | tee -a $out $err
sudo hping3 -$type -i $speed -c $cnt -s $sp -k -p $dp $ip >> $out 2>> $err