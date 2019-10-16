#! /bin/bash
cd ~/sanity_test/shortest-ping
out=~/sanity_test/rs/shortestPing_$(hostname)_$(date -u +"%m%d%H%M").txt
cat $1 | while read ip;do
    ping -W 10 -c 20 -i 0.5 -q $ip >> $out
    echo >> $out
done