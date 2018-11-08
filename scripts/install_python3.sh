sudo apt-get update
sudo apt-get -y --fix-missing install python3 python3-pip python3-setuptools
sudo pip3 install apscheduler pytz 
sudo DEBIAN_FRONTEND=noninteractive apt-get -y install wireshark tshark
echo "wireshark-common wireshark-common/install-setuid boolean true" | sudo debconf-set-selections
sudo DEBIAN_FRONTEND=noninteractive dpkg-reconfigure wireshark-common

