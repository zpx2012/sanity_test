stime=$(date -u +%m%d%H%M)
mkdir ~/packet_trace/loss_$stime
export IFS=","
if [ $1 == s ]
then
cat ~/sanity_test/scripts/1125_aliyun.csv | while read hm ip port; do 
screen -dmS ptr bash -c 'tfile=~/sanity_test_results/ptraceroute_'$hm_$(hostname)_$stime'_server.txt;while true;do date -u +"%Y-%m-%d %H:%M:%S %Z" >> $tfile;sudo paris-traceroute -Q -s 80 -d 33456 -p tcp '$ip' >> $tfile;done;exec bash'
cd ~/packet_trace/loss_$stime
screen -dmS td bash -c 'tcpdump -w tcpdump_'$hm_$(hostname)'_$(date -u +%m%d%H%M%S)utc_server.pcap -G 1 -s 96 -i ens3 -n host '$ip' and tcp port 80;exec bash'
done
elif [ $1 == c ]
then
cat ~/sanity_test/scripts/1125_vultr.csv | while read hm ip port; do 
screen -dmS ptr bash -c 'tfile=~/sanity_test_results/ptraceroute'$(hostname)_$hm_$stime'_client.txt;while true;do date -u +"%Y-%m-%d %H:%M:%S %Z" >> $tfile; paris-traceroute -Q -d 80 -p tcp '$ip' >> $tfile ;done;exec bash'
screen -dmS nethog bash -c 'nethogs -t eth0 &> ~/sanity_test_results/nethogs_$(hostname)_$(date -u +"%m%d%H%M%Sutc").txt'
screen -dmS curl_http_$hm python ~/sanity_test/curl_downloader.py "http://$ip/my.pcap" $ip $hm 0 0 0 33456
cd ~/packet_trace/loss_$stime
screen -dmS td bash -c 'tcpdump -w tcpdump_'$(hostname)_$hm'_$(date -u +%m%d%H%M%S)utc_client.pcap -G 60 -s 96 -i eth0 -n host '$ip' and tcp port 80;exec bash'
done
fi