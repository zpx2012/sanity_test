p#!/bin/bash
# cd ~/sanity_test/
echo -e '\n\n\n\n\n\n\' > sslansw.txt
openssl req -newkey rsa:2048 -new -nodes -x509 -days 3650 -keyout key.pem -out cert.pem < sslansw.txt
rm sslansw.txt
cd httpserver
screen -dmS https-server sudo http-server -S -C ../cert.pem -K ../key.pem -p 443