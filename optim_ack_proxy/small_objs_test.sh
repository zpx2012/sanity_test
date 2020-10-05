#!/bin/bash

log=~/sanity_test/optim_ack_proxy/rs/small_objs_$(date -u +%m%d%H%M).txt
while true; do
	cat ~/sanity_test/optim_ack_proxy/small_objs.txt | while read obj; do
		echo $(date -u --rfc-3339=ns): Curl download start - Normal mode 2>&1 | tee -a $log
		start=$(date +%s.%N)
		curl -LJ4k -o /dev/null --speed-time 5 http://mirror.math.princeton.edu/pub/ubuntu/indices/$obj 2>&1 | tee -a $log
		duration=$(echo "$(date +%s.%N) - $start" | bc)
		echo $(date -u --rfc-3339=ns): Curl download end, duration $duration 2>&1 | tee -a $log
		echo "---" | tee -a $log
		
		sudo /usr/local/squid/sbin/squid -s
		echo $(date -u --rfc-3339=ns): Start squid 2>&1 | tee -a $log
		sleep 2
		
		curl_singlerun=curl_proxy_singlerun_$(date +%s)
		tcpdump_outfile=~/sanity_test/optim_ack_proxy/rs/tcpdump/tcpdump_smallobjs_$(hostname)_mirror.math.princeton.edu_$(date -u +%m%d%H%M).pcap
		screen -dmS td_$(date +%s) bash -c "sudo tcpdump -w $tcpdump_outfile -vv -W 2000 -C 1024 -s 96 -i eth0 -n host mirror.math.princeton.edu and tcp port 80"
		
		echo $(date -u --rfc-3339=ns): Curl download start - Proxy mode 2>&1 | tee -a $log
		start=$(date +%s.%N)
		curl -LJ4k -o /dev/null -x http://127.0.0.1:3128 --speed-time 5 http://mirror.math.princeton.edu/pub/ubuntu/indices/$obj 2>&1 | tee -a $curl_singlerun
		cat $curl_singlerun >> $log
		duration=$(echo "$(date +%s.%N) - $start" | bc)
		echo $(date -u --rfc-3339=ns): Curl download end, duration $duration 2>&1 | tee -a $log
		
		sudo /usr/local/squid/sbin/squid -k shutdown
		echo $(date -u --rfc-3339=ns): Stop squid 2>&1 | tee -a $log
		
		bash ~/sanity_test/ks.sh td
		if ! cat $curl_singlerun | grep -q 'too slow'; then
			rm -v ${tcpdump_outfile}* | tee -a $log
		fi
		
		sudo iptables -F
		sleep 5
		echo $(date -u --rfc-3339=ns): Wait 29s for squid to stop 2>&1 | tee -a $log
		ps -ef | grep squid 2>&1 | tee -a $log
		echo | tee -a $log
		echo | tee -a $log
	done
done
	