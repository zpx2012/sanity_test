#! /bin/bash

ip=$1
hn=$2
dur=$3
lp=$4
stime=$5
log=$6

echo HTTP starts | tee -a $log
screen -ls | tee -a $log
echo ----------------------- | tee -a $log
ip route | tee -a $log
echo ----------------------- | tee -a $log

bash ~/sanity_test/curl_dler.sh $ip $hn $dur http $stime $lp

echo HTTP ends| tee -a $log
echo | tee -a $log