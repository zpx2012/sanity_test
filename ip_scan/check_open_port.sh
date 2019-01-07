for f in pscan_*;do
    cat $f | while IFS=' ' read open tcp port ip tm; do
        # echo $open $tcp $port $ip $tm
        if [ ! -z "$ip" -a "$ip" != " " ]; then
            echo "nc -z -w -5 $ip $port >> co_$f 2>&1"
            nc -z -v -w 5 $ip $port >> co_$f 2>&1
        fi
    done
done