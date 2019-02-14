#!/bin/bash
sudo apt-get update
sudo apt-get install -y gcc python-pip libcurl4-gnutls-dev python-dev libgnutls28-dev screen traceroute dos2unix mtr paris-traceroute hping3 nodejs npm
sudo pip install pycurl numpy scipy shadowsocks psutil
sudo npm install -g http-server
sudo ln -s /usr/bin/nodejs /usr/bin/node
mkdir ~/httpserver
cd ~/httpserver
wget http://108.160.139.201/my.pcap
screen -dmS httpserver sudo http-server -p 80
mkdir ~/sanity_test_results/