#!/bin/bash
cd ~/sanity_test
mkdir rs
for i in {0..9};do
	screen -dmS curl_$i python curl_poll.py con2con/data/$(hostname)_p${i}.csv
done