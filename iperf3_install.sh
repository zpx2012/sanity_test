sudo add-apt-repository -y "ppa:patrickdk/general-lucid"
sudo apt-get -y update
sudo apt-get -y install iperf3
sudo iperf3 -s -4 -b 1m --cport 80 