#! /bin/bash

ip=$1
hn=$2
dur=$3
lp=$4
stime=$5
log=$6

expressvpn protocol tcp
expressvpn connect hk4
sleep 5
echo VPN starts | tee -a $log
screen -ls | tee -a $log
echo ----------------------- | tee -a $log
ip route | tee -a $log
echo ----------------------- | tee -a $log
screen -dmS vpnhttp bash ~/sanity_test/curl_dler.sh $ip $hn $dur vpn $stime $lp
sleep $dur
expressvpn disconnect
screen -S vpnhttp -X quit
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