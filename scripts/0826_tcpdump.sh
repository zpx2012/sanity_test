mkdir ~/packet_trace
screen -dmS test sudo tcpdump -i eth0 -n host 183.79.225.32 -w ~/packet_trace/$(hostname)_yahoo.jp_$(date +"%m%d%H%M").pcap
screen -dmS test sudo tcpdump -i eth0 -n host 210.152.243.234 -w ~/packet_trace/$(hostname)_u-tokyo.ac.jp_$(date +"%m%d%H%M").pcap
screen -dmS test sudo tcpdump -i eth0 -n host 87.250.247.182 -w ~/packet_trace/$(hostname)_yandex.ru_$(date +"%m%d%H%M").pcap
screen -dmS test sudo tcpdump -i eth0 -n host 175.158.11.209 -w ~/packet_trace/$(hostname)_naver.com_$(date +"%m%d%H%M").pcap
screen -dmS test sudo tcpdump -i eth0 -n host 182.162.108.100 -w ~/packet_trace/$(hostname)_daum.net_$(date +"%m%d%H%M").pcap
screen -dmS test sudo tcpdump -i eth0 -n host 184.25.234.183 -w ~/packet_trace/$(hostname)_canada.ca_$(date +"%m%d%H%M").pcap
screen -dmS test sudo tcpdump -i eth0 -n host 52.218.53.27 -w ~/packet_trace/$(hostname)_gov.uk_$(date +"%m%d%H%M").pcap
screen -dmS test sudo tcpdump -i eth0 -n host 137.132.84.218 -w ~/packet_trace/$(hostname)_nus.edu.sg_$(date +"%m%d%H%M").pcap
sleep 120
kill -9 $(pgrep tcpdump)