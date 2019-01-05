#!/bin/bash
while IFS='' read -r line || [[ -n "$line" ]]; do
    echo $line
    sudo ~/masscan/bin/masscan $line -p0-65535 --max-rate 10000 -oL ~/sanity_test_results/pscan_$(echo $line | cut -d'/' -f1).txt
done < "$1"