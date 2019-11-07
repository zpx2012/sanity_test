#! /bin/bash

stime=$(date -u +'%Y%m%d%H%M')

while true;do
echo Try:80
sudo ~/sanity_test/mtr-insertion/mtr -zwnr4T -P 5000 -L 80 -c 10 47.113.86.254 2>&1 | tee -a ~/sanity_test/rs/mtrins_$(hostname)_SZ-ALI-VPN_80_tcp_1_100_${stime}.txt
echo
sleep 1
echo Try:20000
sudo ~/sanity_test/mtr-insertion/mtr -zwnr4T -P 5000 -L 20000 -c 10 47.113.86.254 2>&1 | tee -a ~/sanity_test/rs/mtrins_$(hostname)_SZ-ALI-VPN_20000_tcp_1_100_${stime}.txt
done