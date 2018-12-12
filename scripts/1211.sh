stime=$(date -u +%m%d%H%M)
mkdir ~/packet_trace/loss_$stime
if [ $hostname == "terran" ]
then
screen -dmS bash -c 'tfile=~/sanity_test_results/ptraceroute_sz1-aliyun_terran_$(date -u +%m%d%H%M)_server.txt;while true;do date -u +"%Y-%m-%d %H:%M:%S %Z" >> $tfile; paris-traceroute -Q -d 80 -p tcp 39.108.98.242 >> $time ;done;exec bash'
sudo tcpdump -w ~/packet_trace/tcpdump_sz1-aliyun_terran_$(date -u +%m%d%H%M)utc_server.pcap -G 60 -s 96 -i eth1 -n 39.108.98.242 and tcp port 80
elif [ $hostname == "sz1-aliyun" ]
then
screen -dmS bash -c 'tfile=~/sanity_test_results/ptraceroute_sz1-aliyun_terran_$(date -u +%m%d%H%M)_client.txt;while true;do date -u +"%Y-%m-%d %H:%M:%S %Z" >> $tfile; paris-traceroute -Q -d 80 -p tcp 169.235.31.181 >> $time ;done;exec bash'
screen -dmS python ~/sanity_test/curl_downloader.py 'http://169.235.31.181/sdk-tools-linux-3859397.zip' '169.235.31.181' terran 0 0 0 33456
screen -dmS bash -c 'tcpdump -w ~/packet_trace/tcpdump_sz1-aliyun_terran_$(date -u +%m%d%H%M)utc_client.pcap -G 60 -s 96 -i eth0 -n 169.235.31.181 and tcp port 80;exec bash'
fi