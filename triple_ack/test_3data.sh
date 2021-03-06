#! /bin/bash

cd 	~/sanity_test/triple_ack/
gcc triple_ack.c -o triple_ack -lnfnetlink -lnetfilter_queue
if [[ $1 == s ]]; then
	echo server side
	cat data/$(hostname).csv | while IFS=',' read ip hm port cnt; do 
		echo $ip $hm $port $cnt
		screen -dmS 4data sudo ~/sanity_test/triple_ack/triple_ack $ip $port 4 1
	done
else
	echo client side
	stime=$(date -u +'%Y%m%d%H%M')
	cat data/$(hostname).csv | while IFS=',' read ip hm;do
		echo $ip $hm
		screen -dmS curl_ack bash -c "while true;do bash ~/sanity_test/curl_dler.sh $ip $hm 120 3data $stime 5000;done"
		screen -dmS curl_ctl bash -c "while true;do bash ~/sanity_test/curl_dler.sh $ip $hm 120 ctl $stime 5001;done"
	done
fi