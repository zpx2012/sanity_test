#!/bin/bash

log=~/sanity_test/optim_ack_proxy/rs/small_objs_$(date -u +%m%d%H%M).txt
while true; do
	cat ~/sanity_test/optim_ack_proxy/small_objs.txt | while read obj; do
		sudo /usr/local/squid/sbin/squid -s
		echo $(date -u --rfc-3339=ns): Start squid 2>&1 | tee -a $log
		sleep 2
		echo $(date -u --rfc-3339=ns): Curl download start 2>&1 | tee -a $log
		start=$(date +%s.%N)
		curl -LJ4k -o /dev/null -x http://127.0.0.1:3128 --speed-time 5 http://mirror.math.princeton.edu/pub/ubuntu/indices/$obj 2>&1 | tee -a $log
		duration=$(echo "$(date +%s.%N) - $start" | bc)
		echo $(date -u --rfc-3339=ns): Curl download end, duration $duration 2>&1 | tee -a $log
		sudo /usr/local/squid/sbin/squid -k shutdown
		echo $(date -u --rfc-3339=ns): Stop squid 2>&1 | tee -a $log
		sudo iptables -F
		sleep 5
		echo $(date -u --rfc-3339=ns): Wait 29s for squid to stop 2>&1 | tee -a $log
		ps -ef | grep squid 2>&1 | tee -a $log
	done
done
	