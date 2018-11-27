# cd ~/sanity_test/shadowsocks
# for f in client*.json
# do
#     screen -dmS ss_$f sslocal -c $f
# done
# cd ..
screen -dmS nethog bash -c 'nethogs -t eth0 &> ~/sanity_test_results/nethogs_$(hostname)_$(date -u +"%m%d%H%M%Sutc").txt'
export IFS=","
cd ~/sanity_test
cat scripts/ss_vultr.csv | while read hm ip port; do 
screen -dmS curl_http_$hm python curl_downloader.py "http://$ip/my.pcap" $ip $hm 0 500k 0
screen -dmS curl_ss_$hm python curl_downloader.py "http://$ip/my.pcap" $ip $hm 0 500k $port
done