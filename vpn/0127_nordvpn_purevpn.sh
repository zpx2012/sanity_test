#! /bin/bash

stime=$(date -u +'%Y%m%d%H%M')
log=~/sanity_test/rs/screenlog_0127nordpure_$(hostname)_$(date -u +'%Y%m%d%H%M')
mkdir -p ~/sanity_test/rs
while true;do
    cat ~/sanity_test/vpn/data/$(hostname).csv | while IFS=',' read ip hn lp vpn; do
		bash ~/sanity_test/curl_dler.sh $ip $hn 30 http $stime $lp
        bash ~/sanity_test/vpn/${vpn}.sh $ip $hn 30 $((lp+1)) $stime $log HK
        bash ~/sanity_test/vpn/${vpn}.sh $ip $hn 30 $((lp+2)) $stime $log TW
        bash ~/sanity_test/vpn/${vpn}.sh $ip $hn 30 $((lp+3)) $stime $log JP
        bash ~/sanity_test/vpn/${vpn}.sh $ip $hn 30 $((lp+4)) $stime $log US
    done
done
