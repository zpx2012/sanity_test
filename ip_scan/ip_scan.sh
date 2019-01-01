#!/bin/bash
while IFS='' read -r line || [[ -n "$line" ]]; do
    sudo traceroute -A -p 80 -P tcp -f 4 -m 25 $line >> "$2" 2>&1
    echo >> "$2"
done < "$1"