#!/bin/bash
while IFS='' read -r line || [[ -n "$line" ]]; do
    echo $line
    sudo ~/masscan/bin/masscan $line --show closed -p0-65535 --rate $2 --append-output -oL ~/sanity_test_results/pscan_$(echo $line | cut -d'/' -f1)_$(echo $line | cut -d'/' -f2)_$(hostname).txt
done < "$1"