#!/bin/bash
i=0
n=`shuf -i 1024-65535 -n 1`
cd ~/sanity_test/ip_scan
gcc open_thrput.c -o open_thrput.o
cd ~/sanity_test/ip_scan/data/0222_mtr
for f in $(for n in $(seq 1 1000|shuf);do sed $n'!d' file_list;done);do 
    if (( i < 300 )); then
    screen -dmS perfile_$n bash ~/sanity_test/ip_scan/0222_perfile.sh $f $n
    ((n+=20))
    ((i++))
    fi
done