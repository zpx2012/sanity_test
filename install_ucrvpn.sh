cd ~
sudo apt-get install -y libpangox-1.0-0 lib32z1 lib32ncurses5
wget "http://101.133.171.40/anyconnect-linux64-4.6.02074-predeploy-k9.tar.gz"
tar -xzvf anyconnect-linux64-4.6.02074-predeploy-k9.tar.gz
cd anyconnect-linux64-4.6.02074/vpn
printf "y\n" | sudo ./vpn_install.sh