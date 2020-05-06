#! /bin/bash
cd 	~/sanity_test/triple_ack/
gcc triple_ack.c -o triple_ack -lnfnetlink -lnetfilter_queue
cd data
if [[ $1 == s ]]; then
	cat $(hostname).csv | while read ip hm port cnt; do 
		screen -dmS 4data sudo ./triple_ack $ip $port 4 1
	done
else
	stime=$(date -u +'%Y%m%d%H%M')
	cat $(hostname).csv | while read ip hm;do
		screen -dmS curl_ack bash -c "while true;do bash ~/sanity_test/curl_dler.sh $ip $hm 3data 120 https $stime 5000;done"
		sleep 120
		screen -dmS curl_ctl bash -c "while true;do bash ~/sanity_test/curl_dler.sh $ip $hm ctl 120 https $stime 5001;done"
	done
fi