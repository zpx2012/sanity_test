#! /bin/bash

url=$1
hn=$2
dur=$3
prtcl=$4
stime=$5
lp=$6
ip=$7
dm=$8
out=~/sanity_test/rs/curl_$(hostname)_${hn}_${prtcl}_${stime}.txt
singlerun=singlerun_${hn}_${prtcl}_$(date +%s)
echo Start: $(date -u +'%Y-%m-%d %H:%M:%S') >> $out
# while true; do
for i in 1 2 3 4 5 6 7 8 9 10;do
	curl -LJv4k -o /dev/null --resolve $dm:443:$ip --local-port $lp --limit-rate 500k -m $dur --speed-time 120 $url 2>&1 | tee $singlerun
	if ! cat $singlerun | grep -q 'Address already in use'; then
		cat $singlerun >> $out
		rm $singlerun
		break
	fi
	sleep 1 
	# cat singlerun >> $out
	# echo "Retry after 1 second" | tee -a $out
done
echo >> $out