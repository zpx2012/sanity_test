#!/bin/bash
cd ~/sanity_test/shadowsocks/
for f in *_client.json;do
screen -dmS ss_$f sslocal -c $f
done