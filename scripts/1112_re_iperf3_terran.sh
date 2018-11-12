#!/bin/bash
for ip in 39.104.139.16 39.108.98.242
do
screen -dmS re_iperf3 bash ~/sanity_test/scripts/iperf3_client.sh $ip 5201 regular
screen -dmS re_iperf3 bash ~/sanity_test/scripts/iperf3_client.sh $ip 80 root
done