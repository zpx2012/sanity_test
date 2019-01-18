#!/bin/bash

n=33456
cd ~/sanity_test_results/closed_test
rm via4134.txt
for f in pscan_*.txt;do
    screen -dmS 24_$n bash check_24_file.sh $f $n
    ((n+=5))
done