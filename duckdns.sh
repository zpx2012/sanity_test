#!/bin/bash
out=~/sanity_test/rs/duck_$(hostname).log
mv ~/sanity_test/rs/duck.log $out
while true;do
    echo url="https://www.duckdns.org/update?domains="$(hostname| tr '[:upper:]' '[:lower:]')"-kayodi&token=d349b351-647b-4430-807e-08ad5adacb74&ip=" | curl -k -K - >> $out
    echo | tee -a $out
    date -u +"%Y-%m-%d %H:%M:%S UTC" | tee -a $out
    nslookup $(hostname| tr '[:upper:]' '[:lower:]')-kayodi.duckdns.org | tee -a $out
    curl --speed-time 120 ipinfo.io | tee -a $out
    echo | tee -a $out
    sleep 60
done