#! /bin/bash

vpn=$1

stime=$(date -u +'%Y%m%d%H%M')
log=~/sanity_test/rs/screenlog_0206astrill_http_$(hostname)_$(date -u +'%Y%m%d%H%M')
mkdir -p ~/sanity_test/rs
while true;do
    cat ~/sanity_test/vpn/data/0206/$(hostname).csv | while IFS=',' read ip hn lp; do
		bash ~/sanity_test/curl_dler.sh $ip $hn 60 ${vpn}_http $stime $lp
    done
done
