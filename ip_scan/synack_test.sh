#!/bin/bash
i=0
for ip in 103.44.144.168 103.44.144.186 103.44.144.176 103.44.144.83 103.44.144.14 103.44.144.10 122.10.136.32 122.10.136.103 122.10.136.104 43.225.180.30 43.225.180.40 43.225.180.38 43.225.180.24 43.225.180.226 43.225.180.131
do
echo "sudo tcpdump -w tcpdump_${ip}_$(hostname)_"'%m%d%H%M%S%z'"utc.pcap -G 60 -s 96 -i eth1 -n host $ip and tcp port 80;exec bash" > tmp$((++i)).sh
screen -dmS td_$ip bash tmp$i.sh
screen -dmS cl_$ip bash -c "while true;do ./sanity_test/ip_scan/synack_client $ip 80;done"
screen -dmS tr_$ip bash -c "sudo traceroute -A -p 80 -P tcp -f 4 -m 25 $ip >> ~/sanity_test_results/tr_terran_${ip}_80.txt 2>&1" 
done