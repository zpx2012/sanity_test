#! /bin/bash

cd ~/sanity_test/vpn/
screen -dmS http bash 0206_http.sh
sleep 60
screen -dmS vpn bash 0206_vpn.sh $1 $2 