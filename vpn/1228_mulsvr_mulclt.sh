#! /bin/bash

stime=$(date -u +'%Y%m%d%H%M')
log=~/sanity_test/rs/screenlog_1228MulsvrMulclt_$(hostname)_$(date -u +'%Y%m%d%H%M')
mkdir -p ~/sanity_test/rs
while true;do
    cat ~/sanity_test/vpn/data/$(hostname).csv | while IFS=',' read ip hn lp; do
        bash ~/sanity_test/vpn/ucrvpn_roundrobin.sh $ip $hn 60 $lp $stime $log
    done
done