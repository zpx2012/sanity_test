#! /bin/bash
cd ~/sanity_test/shortest-ping
out=~/sanity_test/rs/shortestPing_$(hostname)_$(date -u +"%m%d%H%M").txt
cat $1 | while read ip;do
    ping -W 10 -c 40 -i 0.2 -q $ip 2>&1 | tee -a $out
    echo >> $out
done