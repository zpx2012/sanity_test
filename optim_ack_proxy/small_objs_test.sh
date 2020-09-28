#!/bin/bash

log=~/sanity_test/optim_ack_proxy/small_objs_$(date -u +%m%d%H%M).txt
while true; do
	cat ~/sanity_test/optim_ack_proxy/small_objs.txt | while read obj; do
		sudo /usr/local/squid/sbin/squid -s
		echo $(date -u): Start squid 2>&1 | tee -a $log
		sleep 2
		echo $(date -u): Curl download start 2>&1 | tee -a $log
		curl -LJ4k -o /dev/null -x http://127.0.0.1:3128 --speed-time 120 http://mirror.math.princeton.edu/pub/ubuntu/indices/$obj 2>&1 | tee -a $log
		echo $(date -u): Curl download end 2>&1 | tee -a $log
		sudo /usr/local/squid/sbin/squid -k shutdown
		echo $(date -u): Stop squid 2>&1 | tee -a $log
		sudo iptables -F
		sleep 29
		echo $(date -u): Wait 29s for squid to stop 2>&1 | tee -a $log
		ps -ef | grep squid 2>&1 | tee -a $log
	done
done
	