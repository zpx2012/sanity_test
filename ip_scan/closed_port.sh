#!/bin/bash

#for every ip/port pair:
#   1.check via 4134: paris-traceroute
#   2.check connectivity: nc - refused
#   3.hping3 while loop
i=0
n=33456
cd ~/sanity_test_results/closed_test
for f in pscan_*.txt;do
    cat $f | grep close | sort -k 4,4 -u | head -3 | while IFS=' ' read closed tcp port ip ts; do
        optr=`sudo paris-traceroute -Q -s $((n+i)) -d $port -p tcp -f 4 -m 25 $ip`
        echo $optr
        rt=`echo $optr | grep (202.97.)` 
        if [ ! -z "$rt" -a "$rt" != " " ]; then
            rt=`nc -zv $ip $port | grep refused`
            if [ ! -z "$rt" -a "$rt" != " " ]; then            
                echo $ip $port  
                out=hping3_closed_${ip}_${port}.txt
                screen -dmS hping3_$ip bash -c "while true;do date -u +"'"%Y-%m-%d %H:%M:%S %Z"'" >> $out;hping3 -SA -i 1 -c 60 -s $((n+i)) -p $port $ip >> $out 2>&1;done"
                ((i++))
            fi
        fi
    done
done