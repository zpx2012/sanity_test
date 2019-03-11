stime=$(date -u +%m%d%H%M)
mkdir ~/packet_trace/loss_$stime
if [ "$(hostname)" == "terran" ]
then
screen -dmS ptr bash -c 'tfile=~/sanity_test/rs/ptraceroute_sz1-aliyun_terran_$(date -u +%m%d%H%M)_server.txt;while true;do date -u +"%Y-%m-%d %H:%M:%S %Z" >> $tfile;sudo paris-traceroute -Q -d 33456 -p tcp 39.108.98.242 >> $tfile;done;exec bash'
cd ~/packet_trace/loss_$stime
screen -dmS td bash -c 'sudo tcpdump -w tcpdump_sz1-aliyun_terran_$(date -u +%m%d%H%M)utc_server.pcap -G 60 -s 96 -i eth1 -n host 39.108.98.242 and tcp port 80;exec bash'
elif [ "$(hostname)" == "sz1-aliyun" ]
then
screen -dmS ptr bash -c 'tfile=~/sanity_test/rs/ptraceroute_sz1-aliyun_terran_$(date -u +%m%d%H%M)_client.txt;while true;do date -u +"%Y-%m-%d %H:%M:%S %Z" >> $tfile; paris-traceroute -Q -d 80 -p tcp 169.235.31.181 >> $tfile ;done;exec bash'
screen -dmS curl python ~/sanity_test/curl_downloader.py 'http://169.235.31.181/sdk-tools-linux-3859397.zip' '169.235.31.181' terran 0 0 0 33456
cd ~/packet_trace/loss_$stime
screen -dmS td bash -c 'tcpdump -w tcpdump_sz1-aliyun_terran_$(date -u +%m%d%H%M)utc_client.pcap -G 60 -s 96 -i eth0 -n host 169.235.31.181 and tcp port 80;exec bash'
fi
