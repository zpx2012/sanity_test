#!/bin/bash
cd ~/sanity_test/ip_scan
gcc open_thrput.c -o open_thrput.o
i=0
n=`shuf -i 1024-65535 -n 1`
mtr=~/mtr-modified/mtr
tf=test_$(date -u +"%m%d%H%M")
cat ~/sanity_test/ip_scan/data/0222_mtr/via4134.txt | while IFS=' ' read ip port; do
    if ((i < 3));then
        sudo $mtr -zwnr4T -P $port $ip > otr_$ip 2>&1
        cat otr_$ip
        rt=`cat otr_$ip | grep -e '202\.97\.\|AS4134' | wc -l`
        # if [ ! -z "$rt" -a "$rt" != " " ]; then
        if (( $rt > 2 ));then
            echo $ip $port >> $tf
            screen -dmS td_$ip bash ~/sanity_test/ip_scan/tcpdump_whole.sh $ip $port $ip
            screen -dmS opt_$ip bash -c "while true;do ~/sanity_test/ip_scan/open_thrput.o $ip $port $n;done;exec bash"
            ((i++))
            ((n++))
        fi
    else
        break
    fi
done
if [ -s $tf ];then
    screen -dmS mtr bash ~/sanity_test/ip_scan/0222_mtr_poll.sh $tf $mtr
fi
exec bash
# 
# for f in $(for n in $(seq 1 1000|shuf);do sed $n'!d' file_list;done);do 
#     if (( i < 20 )); then
#     screen -dmS perfile_$n bash ~/sanity_test/ip_scan/0222_perfile.sh $f $n
#     ((n+=20))
#     ((i++))
#     fi
# done