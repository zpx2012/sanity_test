#!/bin/bash
ip=$1
hn=$2
http_sp=$3
https_sp=$4
ss_lp=$5
ss_sp=$6
iperf_dp=$7
iperf_sp=$8
url="${ip}/my.pcap"
start=$(date -u +"%m%d%H%Mutc")
while true;do
echo http $http_sp
curl -LJv4k -o /dev/null --limit-rate 750k -m 20 --speed-time 120 --local-port $http_sp http://$url 2>&1 | tee -a ~/sanity_test/rs/curl_$(hostname)_${hn}_http_${start}_${http_sp}.txt 
echo
echo https $https_sp
curl -LJv4k -o /dev/null --limit-rate 750k -m 20 --speed-time 120 --local-port $https_sp https://$url 2>&1 | tee -a ~/sanity_test/rs/curl_$(hostname)_${hn}_https_${start}_${https_sp}.txt
echo
echo ss $ss_sp
curl -LJv4k -o /dev/null --limit-rate 750k -m 20 --speed-time 120 --local-port $ss_sp --socks localhost:$ss_lp 2>&1 http://$url | tee -a ~/sanity_test/rs/curl_$(hostname)_${hn}_ss_${start}_${ss_sp}.txt
echo
echo iperf3 $iperf_sp
sudo iperf3 -c $ip -p $iperf_dp -B $(ip route get 8.8.8.8 | head -n1 | awk -- '{print $5x}') --cport $iperf_sp -b 750K -f K -t 20 -4VR --logfile ~/sanity_test/rs/iperf3_$(hostname)_${hn}_${iperf_dp}_${iperf_sp}_${start}.txt
done