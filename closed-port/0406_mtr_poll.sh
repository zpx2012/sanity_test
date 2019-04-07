f=$1
mtr=$2
start=$(date -u +"%m%d%H%M")
while true;do
    cat $f | while IFS=',' read ip dp sp; do
        echo $ip 
        sudo $mtr -zwnr4 -A -P $dp -L $sp -c 60 $ip 2>&1 | tee -a ~/sanity_test/rs/mtr_$(hostname)_${ip}_${dp}_ack_1_60_${start}.txt
    done
done
exec bash   