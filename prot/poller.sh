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
curl -LJv4k -o /dev/null --limit-rate 750k -m 10 --speed-time 120 --local-port $http_sp 120 http://$url 2>&1 | tee -a ~/sanity_test/rs/curl_$(hostname)_${hn}_http_${start}_${http_sp}.txt 
curl -LJv4k -o /dev/null --limit-rate 750k -m 10 --speed-time 120 --local-port $http_sp 120 https://$url 2>&1 | tee -a ~/sanity_test/rs/curl_$(hostname)_${hn}_https_${start}_${https_sp}.txt 
curl -LJv4k -o /dev/null --limit-rate 750k -m 10 --speed-time 120 --local-port $ss_sp --socks localhost:$ss_lp 2>&1 http://$url | tee -a ~/sanity_test/rs/curl_$(hostname)_${hn}_ss_${start}_${ss_sp}.txt 
sudo iperf3 -c $ip -p $iperf_dp --cport iperf_sp -b 750K -f K -t 10 -4Vd --logfile ~/iperf3_$(hostname)_$3_$1_$startime.txt     
done