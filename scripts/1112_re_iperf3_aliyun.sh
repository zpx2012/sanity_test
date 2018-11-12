screen -dmS curl_vultr python ~/sanity_test/curl_downloader.py 'http://169.235.31.181/my.pcap' '169.235.31.181' clean terran 0 750k
screen -dmS root_iperf iperf3 -s -p 80
su regular
script /dev/null
screen -dmS rgl_iperf iperf3 -s