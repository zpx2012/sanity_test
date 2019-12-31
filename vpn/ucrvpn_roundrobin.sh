#! /bin/bash

ip=$1
hn=$2
dur=$3
lp=$4
stime=$5
log=$6

echo For $hn $ip $dur $lp | tee -a $log
for i in 1 2 3 4 5 6 7 8 9 10;do
    echo "Try" $i | tee -a $log
    date +%s | tee -a $log
    screen -dmS vpn bash -c 'printf "0\npzhu011\nWuyaowang:2234\n" | /opt/cisco/anyconnect/bin/vpn -s connect vpn.ucr.edu > vpn_output'
    sleep 5
    cat vpn_output | tee -a $log
    if cat vpn_output | grep -q 'state: Connected'; then
        date +%s | tee -a $log
        echo VPN starts | tee -a $log
        screen -ls | tee -a $log
        echo ----------------------- | tee -a $log
        ip route | tee -a $log
        echo ----------------------- | tee -a $log
        screen -dmS vpnhttp bash ~/sanity_test/curl_dler.sh $ip $hn $dur vpn $stime $lp
        sleep $dur
        /opt/cisco/anyconnect/bin/vpn disconnect
        screen -S vpnhttp -X quit
        screen -S vpn -X quit
        echo VPN ends | tee -a $log
        echo | tee -a $log

        screen -dmS http bash ~/sanity_test/curl_dler.sh $ip $hn $dur http $stime $lp
        echo HTTP starts | tee -a $log
        screen -ls | tee -a $log
        echo ----------------------- | tee -a $log
        ip route | tee -a $log
        echo ----------------------- | tee -a $log
        sleep $dur
        screen -S http -X quit 
        echo HTTP ends| tee -a $log
        echo | tee -a $log
        break
    fi
done