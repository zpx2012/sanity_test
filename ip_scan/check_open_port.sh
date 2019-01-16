for f in pscan_*;do
    cat $f | while IFS=' ' read open tcp port ip tm; do
        # echo $open $tcp $port $ip $tm
        if [ ! -z "$ip" -a "$ip" != " " ]; then
            echo $ip $port
            nc -z -v -w 5 $ip $port | grep refused &> /dev/null
            if [ $? == 0 ] then
            echo $ip $port >> $1
        fi
    done
done