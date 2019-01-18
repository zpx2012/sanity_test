#!/bin/bash

#for every ip/port pair:
#   1.check via 4134: paris-traceroute
#   2.check connectivity: nc - refused
#   3.hping3 while loop
i=0
n=33456
cd ~/sanity_test_results/closed_test
for f in pscan_*.txt;do
    cat $f | grep close | awk '!seen[$4]++' | head -20 | while IFS=' ' read closed tcp port ip ts; do
        if (( i < 5 ));then
            sudo paris-traceroute -Q -s $n -d $port -p tcp -f 4 -m 25 $ip > optr 2>&1
            cat optr
            rt=`cat optr | grep \(202.97.` 
            if [ ! -z "$rt" -a "$rt" != " " ]; then
                echo $ip $port >> via4134.txt
                hping3 -SA -i 1 -c 10 -s $n -p $port $ip > onc 2>&1
                cat onc
                # rt=`cat onc | grep '100% packet loss'`
                if ! cat onc | grep -q '100% packet loss'; then            
                    echo $ip $port  
                    screen -dmS hping3_$ip bash hping3.sh $ip $port $n
                    ((i++))
                    ((n++))
                fi
            fi
        fi
    done
done