#! /bin/bash

stime=$(date -u +'%Y%m%d%H%M')
log=~/sanity_test/rs/screenlog_1230terran_$(hostname)_$(date -u +'%Y%m%d%H%M')
mkdir -p ~/sanity_test/rs
while true;do
    bash ~/sanity_test/vpn/openvpn_roundrobin.sh 169.235.31.181 terran 60 5000 $stime $log terran-client1.ovpn
done