#!/bin/bash
i=0
n=`shuf -i 1024-65535 -n 1`
cd ~/sanity_test/ip_scan/data/sp20
for f in $(for n in $(seq 1 1000|shuf);do sed $n'!d' file_list;done);do 
    if (( i < 150 )); then
    screen -dmS 24_$n bash ~/sanity_test/ip_scan/path_col_l2_0130.sh $f $n
    ((n+=20))
    ((i++))
    else
        # i=(`screen -ls | grep 'in /var/run/screen/S-root'`)
        sleep 360
        i=0
    fi
done