#!/bin/bash
i=0
n=33456
stime=$(date -u +%m%d%H%M)
mkdir ~/packet_trace/loss_$stime
gcc ~/sanity_test/ip_scan/synack_client.c -o ~/sanity_test/ip_scan/synack_client
cat $1 | while IFS=' ' read ip port; do
    sudo paris-traceroute -Q -s $((n+i)) -d $port -p tcp -f 4 -m 25 $ip > ~/sanity_test/rs/tr_terran_${ip}_${port}_$((n+i)).txt 2>&1
    rt=$(cat ~/sanity_test/rs/tr_terran_${ip}_${port}_$((n+i)).txt | grep 202.97)
    if [ ! -z "$rt" -a "$rt" != " " ]; then
        echo $ip $port  
        screen -dmS ptr_$ip bash ~/sanity_test/ip_scan/ptr.sh $ip $port $((n+i))
        cd ~/packet_trace/loss_$stime
        echo "sudo tcpdump -w tcpdump_${ip}_${port}_$(hostname)_"'%m%d%H%M%S%z'"utc.pcap -G 60 -s 96 -i $2 -n host $ip and tcp port $port;exec bash" > tmp$i.sh
        screen -dmS td_$ip bash tmp$i.sh
        screen -dmS cl_$ip bash -c "while true;do ~/sanity_test/ip_scan/synack_client $ip $port $((n+i));done"
        ((i++))
        # screen -dmS tr_$ip bash -c "sudo traceroute -A -p 80 -P tcp -f 4 -m 25 $ip >> ~/sanity_test/rs/tr_terran_${ip}_80.txt 2>&1" 
    fi
done