ip=$1
hn=$2
dur=$3
prtcl=$4
stime=$5
lp=$6
out=~/sanity_test/rs/curl_$(hostname)_${hn}_${prtcl}_${stime}.txt
echo Start: $(date -u +'%Y-%m-%d %H:%M:%S') >> $out
curl -LJv4k -o /dev/null --local-port $lp --limit-rate 500k -m $dur --speed-time 120 http://$ip/my.mp4 2>&1 | tee -a $out