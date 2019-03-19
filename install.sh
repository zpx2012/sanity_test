#!/bin/bash
sudo apt-get update
sudo apt-get install -y gcc python-pip libcurl4-gnutls-dev python-dev libgnutls28-dev screen traceroute dos2unix paris-traceroute hping3 nodejs npm automake
sudo pip install pycurl numpy scipy shadowsocks psutil
mkdir ~/sanity_test/rs/