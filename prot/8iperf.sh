#!/bin/bash
for i in {0..7};do
    screen -dmS iperf3_$i bash -c "sudo iperf3 -s -p $((5201+i)) -f K | tee -a ~/sanity_test/rs/iperf3_$(hostname)_$((5201+i))_$(date -u +"%m%d%H%M")_server.txt"
done