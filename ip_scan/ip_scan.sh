#!/bin/bash
while IFS='' read -r line || [[ -n "$line" ]]; do
    echo $line
    sudo paris-traceroute -Q -d 80 -p tcp -f 4 -m 25 $line >> "$2" 2>&1
    echo >> "$2"
done < "$1"