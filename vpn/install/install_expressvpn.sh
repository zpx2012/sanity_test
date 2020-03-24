cd ~
sudo apt install -y expect
wget "https://101.133.171.40/expressvpn_2.4.4.19-1_amd64.deb"
sudo dpkg -i expressvpn_2.4.4.19-1_amd64.deb
expressvpn preferences set network_lock off