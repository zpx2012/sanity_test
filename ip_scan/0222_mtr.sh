#!/bin/bash
cd ~/sanity_test/ip_scan
gcc open_thrput.c -o open_thrput.o
cd ~/sanity_test/ip_scan/data/0222_mtr

i=0
n=`shuf -i 1024-65535 -n 1`
mtr=~/mtr-modified/mtr
trfile=~/sanity_test_results/$(echo ${f/pscan/tr}| sed -e 's/.txt//g')_$(hostname)_$(date -u +"%m%d%H%M").txt
tf=test_$(date -u +"%m%d%H%M")
cat via4134.txt | while IFS=' ' read closed tcp port ip ts; do
    if ((i < 3));then
        sudo $mtr -zwnr4T -P $port -c 100 $ip 2>&1 otr_$ip
        cat otr_$ip
        cat otr_$ip >> $trfile
        rt=`cat otr_$ip | grep -e '202\.97\.\|AS4134' | wc -l`
        # if [ ! -z "$rt" -a "$rt" != " " ]; then
        if (( $rt > 2 ));then
            ((i++))
            echo $ip $port >> $tf
            screen -dmS td_$ip bash ~/sanity_test/ip_scan/tcpdump_whole.sh $ip $port $ip
            screen -dmS opt_$ip bash -c "while true;do ./open_thrput.o $ip $port $n;done"
        fi
    else
        screen -dmS mtr_$ip bash ~/sanity_test/scripts/tmpl_mtr_tcp_1_100.sh $ip $port $mtr
        break
    fi
done


# 
# for f in $(for n in $(seq 1 1000|shuf);do sed $n'!d' file_list;done);do 
#     if (( i < 20 )); then
#     screen -dmS perfile_$n bash ~/sanity_test/ip_scan/0222_perfile.sh $f $n
#     ((n+=20))
#     ((i++))
#     fi
# done