#!/bin/bash
start=$(date -u +"%m%d%H%Mutc")
while true;do
    cat ~/sanity_test/alexa/urls.csv | while IFS=',' read url hn tag ip;do
    echo
    echo $tag
    echo
    curl -LJ4kv -o /dev/null --limit-rate 500k -m 10 --speed-time 120 --resolve "$hn:443:$ip" "$url" 2>&1 | tee -a ~/sanity_test/rs/curl_$(hostname)_${tag}_https_${start}.txt
    done
done