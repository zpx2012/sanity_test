#!/bin/bash
for ip in 13.58.240.169 206.189.156.225 142.93.173.127 52.62.247.88 52.90.209.164 139.59.30.168 18.179.203.167 52.67.194.125 34.217.77.209 13.48.6.9 13.125.44.223 159.203.44.95 138.197.195.141 134.209.25.12 104.248.124.123 185.180.199.28; do
    for i in 0 1 2 3 4;do
        nc -zv -p 10000 $ip 8888
        nc -zv -p 10001 $ip 9999
    done
done