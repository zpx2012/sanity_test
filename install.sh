#!/bin/bash
sudo apt-get update
sudo apt-get install -y gcc python-pip libcurl4-gnutls-dev python-dev libgnutls28-dev screen traceroute dos2unix paris-traceroute hping3 nodejs npm automake
sudo pip install pycurl numpy scipy shadowsocks psutil
sudo npm install -g http-server
sudo ln -s /usr/bin/nodejs /usr/bin/node
mkdir ~/httpserver
mv ~/sanity_test/my.pcap ~/httpserver 
#screen -dmS httpserver sudo http-server -p 80
mkdir ~/sanity_test/rs/
bash ~/sanity_test/mtr-modified/install.sh