#!/bin/bash
while true;do
    echo url="https://www.duckdns.org/update?domains="$(hostname| tr '[:upper:]' '[:lower:]')"-kayodi&token=d349b351-647b-4430-807e-08ad5adacb74&ip=" | curl -k -K - >> ~/sanity_test/duck.log
    date -u +"%Y-%m-%d %H:%M:%S UTC" >> ~/sanity_test/duck.log
    nslookup $(hostname| tr '[:upper:]' '[:lower:]')-kayodi.duckdns.org >> ~/sanity_test/duck.log
    sleep 60
done