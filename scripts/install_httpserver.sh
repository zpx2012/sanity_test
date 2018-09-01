sudo apt-get update
sudo apt-get -y install nodejs npm
sudo npm install -g http-server
sudo ln -s /usr/bin/nodejs /usr/bin/node
mkdir httpserver
cd httpserver
screen -dmS httpserver sudo http-server -p 80