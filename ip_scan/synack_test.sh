#!/bin/bash
i=0
stime=$(date -u +%m%d%H%M)
mkdir ~/packet_trace/loss_$stime
gcc ~/sanity_test/ip_scan/synack_client.c -o ~/sanity_test/ip_scan/synack_client
cat $1 | while IFS='' read -r ip || [[ -n "$ip" ]]; do
    sudo traceroute -A -p 80 -P tcp -f 4 -m 25 $ip >> ~/sanity_test_results/tr_terran_${ip}_80.txt 2>&1
    rt=$(cat ~/sanity_test_results/tr_terran_${ip}_80.txt | grep 202.97)
    if [ ! -z "$rt" -a "$rt" != " " ]; then
        cd ~/packet_trace/loss_$stime
        echo "sudo tcpdump -w tcpdump_${ip}_$(hostname)_"'%m%d%H%M%S%z'"utc.pcap -G 60 -s 96 -i eth1 -n host $ip and tcp port 80;exec bash" > tmp$((++i)).sh
        screen -dmS td_$ip bash tmp$i.sh
        screen -dmS cl_$ip bash -c "while true;do ~/sanity_test/ip_scan/synack_client $ip 80;done"
        # screen -dmS tr_$ip bash -c "sudo traceroute -A -p 80 -P tcp -f 4 -m 25 $ip >> ~/sanity_test_results/tr_terran_${ip}_80.txt 2>&1" 
    fi
done