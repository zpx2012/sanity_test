sudo npm install -g http-server
sudo ln -s /usr/bin/nodejs /usr/bin/node
mkdir -p ~/httpserver
cd ~/httpserver
curl -o my.mp4 "http://mirror.math.princeton.edu/pub/ubuntu-iso/xenial/ubuntu-16.04.6-server-amd64.iso"
screen -dmS httpserver sudo http-server -p 80