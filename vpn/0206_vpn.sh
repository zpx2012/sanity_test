#! /bin/bash

server_ip=$1
proxy_local_sp=$2
proxy_foreg_sp=$3

stime=$(date -u +'%Y%m%d%H%M')
log=~/sanity_test/rs/screenlog_0206astrill_vpn_$(hostname)_$(date -u +'%Y%m%d%H%M')
mkdir -p ~/sanity_test/rs
while true;do
    cat ~/sanity_test/vpn/data/0206/$(hostname).csv | while IFS=',' read ip hn lp; do
		screen -dmS vpn_$hn bash ~/sanity_test/curl_dler_wProxy.sh $ip $hn 60 astrillvpn $stime $((lp+1)) "http://localhost:${proxy_port}"
        sudo ~/sanity_test/mtr-modified/mtr -zwnr4T -P 443 -c 60 -i 0.5 $ip 2>&1 | tee -a ~/sanity_test/rs/mtrins_$(hostname)_${hn}_${sp}_tcp_05_60_${stime}.txt
    done
done
