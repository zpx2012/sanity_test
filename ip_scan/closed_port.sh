#!/bin/bash

n=33456
cd ~/sanity_test_results/closed_test
for f in pscan_*.txt;do
    screen -dmS 24_$n bash ~/sanity_test/ip_scan/check_24_file.sh $f $n
    ((n+=5))
done