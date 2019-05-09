#!/bin/bash
cd ~/sanity_test/o2c/
dataf=hk/$(hostname).csv

if [ "$(hostname)" == "HKG-CHN-TEC" ];then
	screen -dmS td bash ~/sanity_test/ip_scan/tcpdump_server.sh 80
	screen -dmS sched python sched_curl.py $dataf 65 520 2 s
else
	cat $dataf | while IFS=' ' read ip hn sp dp tm; do
	    screen -dmS td_$hn bash ~/sanity_test/ip_scan/tcpdump_whole.sh $ip $dp $hn c
	done
    screen -dmS sched python sched_curl.py $dataf 65 520 2 c
fi
sleep 172800
bash ~/sanity_test/ks.sh sched
bash ~/sanity_test/ks.sh td
