#!/bin/bash
sudo apt-get update
sudo apt-get install -y curl gcc python-pip python-setuptools libcurl4-gnutls-dev python-dev libgnutls28-dev screen traceroute dos2unix paris-traceroute hping3 nodejs npm automake openvpn
pip install apscheduler pytz wheel pycurl numpy scipy shadowsocks psutil 
mkdir ~/sanity_test/rs/
cd ~/sanity_test/
git submodule update --init --recursive
cd ~/sanity_test/mtr-insertion/ ; git pull origin master; ./install.sh
cd ~/sanity_test/mtr-modified/ ;  git pull origin master; ./install.sh