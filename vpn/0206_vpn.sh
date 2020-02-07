#! /bin/bash

server_ip=$1
proxy_local_sp=$2
vpn=$3

stime=$(date -u +'%Y%m%d%H%M')
log=~/sanity_test/rs/screenlog_0206${vpn}_vpn_$(hostname)_$(date -u +'%Y%m%d%H%M')
mkdir -p ~/sanity_test/rs
while true;do
    cat ~/sanity_test/vpn/data/0206/$(hostname).csv | while IFS=',' read ip hn lp; do
        # screen -dmS mtr_$hn sudo mtr -zwnr4T -P 443 -c 60 -i 0.5 ${server_ip} 2>&1 | tee -a ~/sanity_test/rs/mtrins_$(hostname)_${hn}_$((lp+1))_tcp_05_60_${stime}.txt
		bash ~/sanity_test/curl_dler_wProxy.sh $ip $hn 60 ${vpn}_vpn $stime $((lp+1)) "http://localhost:${proxy_local_sp}"
    done
done
