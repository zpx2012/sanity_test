cd ~
sudo apt install -y expect
sudo apt autoremove expressvpn
wget "http://101.133.171.40/expressvpn_2.4.4.19-1_amd64.deb"
sudo dpkg -i expressvpn_2.4.4.19-1_amd64.deb
expressvpn preferences set network_lock off 2> install_expressvpn_preferences_out
if cat install_expressvpn_preferences_out | grep -q "Please activate your account." ; 
then 
	./sanity_test/vpn/install/expressvpn.exp; 
	expressvpn preferences set network_lock off
fi