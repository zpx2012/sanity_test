f=$1
n=$2
i=0
trfile=~/sanity_test/rs/$(echo ${f/pscan/tr}| sed -e 's/.txt//g')_$(hostname)_$(date -u +"%m%d%H%M").txt
cat $f | while IFS=' ' read closed tcp port ip ts; do
    if ((i < 5));then
        sudo traceroute -A --sport=$n -p $port -T -f 4 -m 25 $ip > otr_$ip
        cat otr_$ip
        cat otr_$ip >> $trfile
        rt=`cat otr_$ip | grep -e '202\.97\.\|AS4134'`
        if [ ! -z "$rt" -a "$rt" != " " ]; then
            ((i++))
            echo $ip $port >> via4134.txt
            hping3 -SA -i u500000 -c 10 -s $n -p $port $ip 2> oncSA_$n;cat oncSA_$n
            hping3 -S -i u500000 -c 10 -s $n -p $port $ip 2> oncS_$n;cat oncS_$n
            # rt=`cat onc | grep '100% packet loss'`
            if ! cat oncSA_$n | grep -q '100% packet loss'; then            
                echo SYNACK: $ip $port  
                screen -dmS hping3_ptr_SA_$ip bash ~/sanity_test/ip_scan/hping3_ptr.sh $ip $port $n SA 1 60
                ((n++))
            elif ! cat oncS_$n | grep -q '100% packet loss'; then
                echo SYN: $ip $port  
                screen -dmS hping3_ptr_S_$ip bash ~/sanity_test/ip_scan/hping3_ptr.sh $ip $port $n S 1 60
                ((n++))
            fi
        fi
    fi
done