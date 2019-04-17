#!/bin/bash
for i in {0..7};do
    screen -dmS iperf3_$i iperf3 -s $((5201+i)) --logfile ~/sanity_test/rs/iperf3_$(hostname)_$((5201+i))_$(date -u +"%m%d%H%M")_server.txt
done