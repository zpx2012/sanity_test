#!/bin/bash

log=~/sanity_test/optim_ack_proxy/small_objs_$(date -u +%m%d%H%M).txt
while true; do
	cat ~/sanity_test/optim_ack_proxy/small_objs.txt | while read obj; do
		sudo /usr/local/squid/sbin/squid -s
		curl -LJv4k -o /dev/null -x http://127.0.0.1:3128 --speed-time 120 http://mirror.math.princeton.edu/pub/ubuntu/indices/$obj 2>&1 | tee -a $log
		sudo /usr/local/squid/sbin/squid -k shutdown
		sudo iptables -F
		sleep 29
	done
done
	