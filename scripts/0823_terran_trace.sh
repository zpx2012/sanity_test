screen -dmS terran bash -c "sudo tcpdump -i eth0 -n host 169.235.31.181 -w sz_client_0823.pcap"
sleep 2
screen -dmS terran bash -c "python ~/sanity_test/curl_downloader.py 'http://169.235.31.181/sdk-tools-linux-3859397.zip' '169.235.31.181' clean terran"
screen -dmS terran bash -c "python ~/sanity_test/mtr_runner.py 169.235.31.181 terran"