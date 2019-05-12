#!/bin/bash
if [ -z "$1" ]
  then
    echo "No argument supplied"
    exit 1
fi
start=$(date -u +"%m%d%H%Mutc")
while true;do
    cat $1 | while IFS=',' read url hn tag ip;do
    echo
    echo $tag
    echo
    curl -LJ4kv -o /dev/null --limit-rate 500k -m 20 --speed-time 120 --resolve "$url" 2>&1 | tee -a ~/sanity_test/rs/curl_$(hostname)_${tag}_https_${start}.txt
    done
done