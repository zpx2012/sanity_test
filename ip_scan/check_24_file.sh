#!/bin/bash

#for every ip/port pair:
#   1.check via 4134: paris-traceroute
#   2.check connectivity: nc - refused
#   3.hping3 while loop
f=$1
n=$2
i=0
cat $f | grep close | awk '!seen[$4]++' | head -20 | while IFS=' ' read closed tcp port ip ts; do
    if (( i < 5 ));then
        sudo paris-traceroute -Q -s $n -d $port -p tcp -f 4 -m 25 $ip > optr 2>&1
        cat optr
        rt=`cat optr | grep \(202.97.` 
        if [ ! -z "$rt" -a "$rt" != " " ]; then
            hping3 -SA -i u500000 -c 10 -s $n -p $port $ip 2>> oncSA 
            hping3 -S -i u500000 -c 10 -s $n -p $port $ip 2>> oncS 
            cat oncSA
            cat oncS
            # rt=`cat onc | grep '100% packet loss'`
            if ! cat oncSA | grep -q '100% packet loss'; then            
                echo SYNACK: $ip $port  
                screen -dmS hping3_SA_$ip bash hping3.sh $ip $port $n SA u500000
                ((i++))
                ((n++))

            elif ! cat oncS | grep -q '100% packet loss'; then
                echo SYN: $ip $port  
                screen -dmS hping3_S_$ip bash hping3.sh $ip $port $n S 1
                ((i++))
                ((n++))
            else
                echo $ip $port >> via4134.txt
            fi
        fi
    fi
done