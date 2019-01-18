#!/bin/bash

#for every ip/port pair:
#   1.check via 4134: paris-traceroute
#   2.check connectivity: nc - refused
#   3.hping3 while loop
i=0
n=33456
cd ~/sanity_test_results/closed_test
for f in pscan_*.txt;do
    cat $f | grep close | sort -k 4,4 -u | head -5 | while IFS=' ' read closed tcp port ip ts; do
        sudo paris-traceroute -Q -s $((n+i)) -d $port -p tcp -f 4 -m 25 $ip | grep 202.97 &> /dev/null
        if [ $? != 0 ]; then
            nc -zv $ip $port | grep refused &> /dev/null
            if [ $? != 0 ];then
            echo $ip $port  
            out=`echo $f | sed -e "s/pscan/hping3_closed/g"`
            hping3 -R -i 1 -c 60 -s 33456 -d
            screen -dmS hping3_$ip bash -c "while true;do date -u +"'"%Y-%m-%d %H:%M:%S %Z"'" >> $out;hping3 -q -SA -i 1 -c 60 -s $((n+i)) -p $port $ip >> $out 2>&1;done"
            ((i++))
            fi
        fi
    done
done