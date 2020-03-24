#! /bin/bash
dir_name=0322_expressvpn
vpnfile=~/sanity_test/0322_expressvpn/expressvpn_wMtr.sh

stime=$(date -u +'%Y%m%d%H%M')
log=~/sanity_test/rs/screenlog_${dir_name}_$(hostname)_$(date +%s)
mkdir -p ~/sanity_test/rs
cd ~/sanity_test/vpn/$dir_name
while true;do
    cat data/$(hostname).csv | while IFS=',' read ip hn lp; do
        bash $vpnfile $ip $hn 30 $((lp+1)) $stime $log hk
        bash $vpnfile $ip $hn 30 $((lp+2)) $stime $log tw
        bash $vpnfile $ip $hn 30 $((lp+3)) $stime $log jp
        bash $vpnfile $ip $hn 30 $((lp+4)) $stime $log us
		bash ~/sanity_test/curl_dler.sh $ip $hn 30 http $stime $lp
    done
done
