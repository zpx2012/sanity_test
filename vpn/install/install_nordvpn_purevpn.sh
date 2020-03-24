#! /bin/bash

cd ~
sudo apt install -y expect
wget "https://101.133.171.40/purevpn_1.2.1_amd64.deb"
sudo dpkg -i purevpn_1.2.1_amd64.deb
purevpn –protocol tcp
./sanity_test/vpn/install/purevpn.exp

wget -qnc "https://101.133.171.40/nordvpn-release_1.0.0_all.deb"
sudo dpkg -i nordvpn-release_1.0.0_all.deb
sudo wget https://repo.nordvpn.com/gpg/nordvpn_public.asc -O - | sudo apt-key add -
sudo apt update
sudo apt install -y nordvpn
nordvpn set protocol tcp
./sanity_test/vpn/install/nordvpn.exp

purevpn -d
