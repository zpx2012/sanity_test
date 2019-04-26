#!/bin/bash
ip=$1
dp=$2
sp=$3
cd ~/sanity_test/ip_scan/
screen -dmS hping_icmp_$ip bash hping3.sh $ip $dp $sp 1 1 60
screen -dmS hping_icmp_$ip bash hping3.sh $ip $dp $sp 2 1 60
screen -dmS hping_icmp_$ip bash hping3.sh $ip $((dp+1)) $((sp+1)) S 1 60
screen -dmS hping_icmp_$ip bash hping3.sh $ip $((dp+2)) $((sp+2)) SA 1 60
screen -dmS hping_icmp_$ip bash hping3.sh $ip $((dp+3)) $((sp+3)) A 1 60
screen -dmS hping_icmp_$ip bash hping3_data.sh $ip $((dp+4)) $((sp+4)) 1 60