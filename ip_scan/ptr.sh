#!/bin/bash
ip=$1
dp=$2
sp=$3
tfile=~/sanity_test/rs/ptraceroute_$(hostname)_${ip}_${dp}_${sp}_$(date -u +%m%d%H%M%S).txt
while true;do 
    date -u +"%Y-%m-%d %H:%M:%S %Z" >> $tfile
    sudo paris-traceroute -Q -s $sp -d $dp -p tcp -f 4 -m 25 $ip >> $tfile
done
exec bash