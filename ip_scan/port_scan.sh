#!/bin/bash
while IFS='' read -r line || [[ -n "$line" ]]; do
    masscan/bin/masscan -p0-65535 --max-rate 1000 --output-format list $line >> "$2" 2>&1
    echo >> "$2"
done < "$1"