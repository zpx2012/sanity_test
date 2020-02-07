#! /bin/bash

stime=$(date -u +'%Y%m%d%H%M')
log=~/sanity_test/rs/screenlog_0206${vpn}_vpn_$(hostname)_$(date -u +'%Y%m%d%H%M')
mkdir -p ~/sanity_test/rs
while true;do
    cat ~/sanity_test/vpn/data/0127$(basename "$(dirname "$filepath")")/$(hostname).csv | while IFS=',' read ip hn lp vpn vpn_ip; do
        screen -dmS mtrins_$hn bash ~/sanity_test/vpn/mtr-ins.sh ~/sanity_test/mtr-insertion/mtr ${vpn_ip} $vpn 443 60 0.5 ${stime}
		bash ~/sanity_test/curl_dler_wProxy.sh $ip $hn 60 ${vpn}_vpn $stime $((lp+1)) "http://localhost:3213"
        bash ~/sanity_test/vpn/mtr-ins.sh mtr ${vpn_ip} $vpn 443 10 0.5 ${stime}
    done
done
