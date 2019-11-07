#! /bin/bash

ip=$1
hn=$2
dur=$3
lp=$4
stime=$5
log=$6
ovpn=$7

echo For $hn $ip $dur $lp | tee -a $log
for i in 1 2 3 4 5 6 7 8 9 10;do
    echo "Try" $i | tee -a $log
    date +%s | tee -a $log
    screen -dmS vpn sudo openvpn --config ~/sanity_test/vpn/$ovpn --log ~/sanity_test/rs/openvpn_output
    sleep 5
    cat ~/sanity_test/rs/openvpn_output | tee -a $log
    if cat ~/sanity_test/rs/openvpn_output | grep -q 'Initialization Sequence Completed'; then
        echo VPN starts | tee -a $log
        date +%s | tee -a $log
        echo ----------------------- | tee -a $log
        ip route | tee -a $log
        echo ----------------------- | tee -a $log
        screen -dmS vpnhttp bash ~/sanity_test/curl_dler.sh $ip $hn $dur vpn $stime $lp
        screen -ls | tee -a $log
        sleep $dur
        /opt/cisco/anyconnect/bin/vpn disconnect
        screen -S vpnhttp -X quit
        sudo killall openvpn
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
