#! /bin/bash

server_ip=$1
proxy_local_sp=$2

stime=$(date -u +'%Y%m%d%H%M')
log=~/sanity_test/rs/screenlog_0206astrill_vpn_$(hostname)_$(date -u +'%Y%m%d%H%M')
mkdir -p ~/sanity_test/rs
while true;do
    cat ~/sanity_test/vpn/data/0206/$(hostname).csv | while IFS=',' read ip hn lp; do
		screen -dmS vpn_$hn bash ~/sanity_test/curl_dler_wProxy.sh $ip $hn 60 astrillvpn $stime $((lp+1)) "http://localhost:${proxy_local_sp}"
        screen -dmS mtr_$hn sudo ~/sanity_test/mtr-insertion/mtr -zwnr4T -P 443 -c 60 -i 0.5 $server_ip 2>&1 | tee -a ~/sanity_test/rs/mtrins_$(hostname)_${hn}_$((lp+1))_tcp_05_60_${stime}.txt
        sleep 60
    done
done
