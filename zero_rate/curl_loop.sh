#! /bin/bash

url=$1
hn=$2
dur=$3
prtcl=$4
stime=$5

out=~/sanity_test/rs/curl_$(hostname)_${hn}_${prtcl}_${stime}.txt
singlerun=singlerun_${hn}_${prtcl}_$(date +%s)
echo Start: $(date -u +'%Y-%m-%d %H:%M:%S') >> $out
# while true; do
	curl -LJv4k -o /dev/null --limit-rate 500k -m $dur --speed-time 120 $url 2>&1 | tee -a $out
	# cat singlerun >> $out
	# echo "Retry after 1 second" | tee -a $out
# done
echo >> $out