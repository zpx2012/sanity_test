f=$1
mtr=$2
start=$(date -u +"%m%d%H%M")
while true;do
    cat $f | while IFS=' ' read ip hn dp sp; do
        echo $ip $hn $dp $sp
        sudo $mtr -zwnr4T -P $dp -L $((sp+1)) -c 60 $ip 2>&1 | tee -a ~/sanity_test/rs/mtrins_$(hostname)_${hn}_${sp}_tcp_1_100_${start}.txt
    done
done
exec bash    