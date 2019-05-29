#!/bin/bash
if [ -z "$1" ]
  then
    echo "No argument supplied"
    exit 1
fi
start=$(date -u +"%m%d%H%Mutc")
mkdir -p ~/sanity_test/rs
while true;do
    cat $1 | while IFS=',' read url tag;do
    echo
    echo $tag
    echo
    out=~/sanity_test/rs/curl_$(hostname)_${tag}_https_${start}.txt
    date -u +"%Y-%m-%d %H:%M:%S" | tee -a $out
    echo >> $out
    curl -LJ4kv -o /dev/null --limit-rate 500k -m 20 --speed-time 120 "$url" 2>&1 | tee -a $out
    done
done