#!/bin/bash
n=$2
cat $1 | while IFS=' ' read ip port; do
    sudo paris-traceroute -Q -s $n -d $port -p tcp -f 4 -m 25 $ip > optr 2>&1
    cat optr
    cat optr >> ~/sanity_test_results/ptr_redo0124_$(hostname)_$(date -u +"%m%d%H%M").txt
    rt=`cat optr | grep \(202.97.`
    if [ ! -z "$rt" -a "$rt" != " " ]; then
        hping3 -SA -i u500000 -c 10 -s $n -p $port $ip 2> oncSA_$n;cat oncSA_$n
        hping3 -S -i u500000 -c 10 -s $n -p $port $ip 2> oncS_$n;cat oncS_$n
        # rt=`cat onc | grep '100% packet loss'`
        if ! cat oncSA_$n | grep -q '100% packet loss'; then            
            echo SYNACK: $ip $port  
            screen -dmS hping3_SA_$ip bash ~/sanity_test/ip_scan/hping3.sh $ip $port $n SA u500000 120
            ((n++))
        elif ! cat oncS_$n | grep -q '100% packet loss'; then
            echo SYN: $ip $port  
            screen -dmS hping3_S_$ip bash ~/sanity_test/ip_scan/hping3.sh $ip $port $n S 1 60
            ((n++))
        else
            echo $ip $port >> via4134.txt
        fi
    fi
done