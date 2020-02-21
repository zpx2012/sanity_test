#! /bin/bash

stime=$(date -u +'%Y%m%d%H%M')
log=~/sanity_test/rs/screenlog_0220_privatevpn_$(hostname)_$(date -u +'%Y%m%d%H%M')
mkdir -p ~/sanity_test/rs
cd ~/sanity_test/vpn/0220_privatevpn
while true;do
    cat data/$(hostname).csv | while IFS=',' read ip hn lp; do
		bash ~/sanity_test/curl_dler.sh $ip $hn 30 http $stime $lp
        bash ~/sanity_test/vpn/openvpn.sh $ip $hn 30 $((lp+1)) $stime $log privatevpn_hk.ovpn
        bash ~/sanity_test/vpn/openvpn.sh $ip $hn 30 $((lp+2)) $stime $log privatevpn_tw.ovpn
        bash ~/sanity_test/vpn/openvpn.sh $ip $hn 30 $((lp+3)) $stime $log privatevpn_jp.ovpn
        bash ~/sanity_test/vpn/openvpn.sh $ip $hn 30 $((lp+4)) $stime $log privatevpn_us.ovpn
    done
done
