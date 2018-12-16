stime=$(date -u +%m%d%H%M)
mkdir ~/packet_trace/loss_$stime
export IFS=","
if [[ $1 == s ]]; then
if [[ $(hostname) == singapore-vultr ]]; then
n=33456
elif [[ $(hostname) == sydney-vultr ]]; then
n=33457
elif [[ $(hostname) == tokyo-vultr ]]; then
n=33457
fi
cd ~/packet_trace/loss_$stime
i=0
cat ~/sanity_test/scripts/1125_aliyun.csv | while read hm ip port; do 
tfile=~/sanity_test_results/ptraceroute_${hm}_$(hostname)_$(date -u +%m%d%H%M%S)utc_server.txt
echo "while true;do date -u +"'"%Y-%m-%d %H:%M:%S %Z"'">> $tfile;sudo paris-traceroute -Q -s 80 -d $n -p tcp $ip >> $tfile;done;exec bash" > tmp$((++i)).sh
screen -dmS ptr_$hm bash tmp$i.sh
echo "sudo tcpdump -w tcpdump_${hm}_$(hostname)_"'$(date -u +%m%d%H%M%S)'"utc_server.pcap -G 1 -s 96 -i ens3 -n host $ip and tcp port 80;exec bash" > tmp$((++i)).sh
screen -dmS td_$hm bash tmp$i.sh
done
elif [[ $1 == c ]]; then
n=33456
i=0
screen -dmS nethog bash -c 'sudo nethogs -t eth0 &> ~/sanity_test_results/nethogs_$(hostname)_$(date -u +"%m%d%H%M%Sutc").txt'
cd ~/packet_trace/loss_$stime
cat ~/sanity_test/scripts/1125_vultr.csv | while read hm ip port; do 
echo $hm
screen -dmS curl_http_$hm python ~/sanity_test/curl_downloader.py "http://$ip/my.pcap" $ip $hm 0 0 0 $((n+i))
tfile=~/sanity_test_results/ptraceroute_$(hostname)_${hm}_$(date -u +%m%d%H%M%S)utc_client.txt
echo "while true;do date -u +"'"%Y-%m-%d %H:%M:%S %Z"'" >> $tfile; sudo paris-traceroute -Q -s $((n+i)) -d 80 -p tcp $ip >> $tfile ;done;exec bash" > ptr$i.sh
screen -dmS ptr_$hm bash ptr$i.sh
echo "sudo tcpdump -w tcpdump_$(hostname)_${hm}_"'$(date -u +%m%d%H%M%S)'"utc_client.pcap -G 60 -s 96 -i eth0 -n host $ip and tcp port 80;exec bash" > td$i.sh
screen -dmS td_$hm bash td$i.sh
((i++))
done
fi