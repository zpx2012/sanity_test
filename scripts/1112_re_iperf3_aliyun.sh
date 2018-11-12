screen -dmS curl_vultr python ~/sanity_test/curl_downloader.py 'http://169.235.31.181/my.pcap' '169.235.31.181' clean terran 0 750k
startime=$(date -u +"%m%d%H%Mutc")
screen -dmS root_iperf iperf3 -s -p 80 -f K --logfile ~/iperf3_server_root_$(hostname)_terran_$startime.txt
sudo -u regular bash -c "script /dev/null;screen -dmS rgl_iperf iperf3 -s -f K --logfile ~/iperf3_server_regular_$(hostname)_terran_$startime.txt"