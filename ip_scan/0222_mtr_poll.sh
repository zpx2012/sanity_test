f=$1
mtr=$2
while true;do
    cat $f | while IFS=' ' read ip port; do
        sudo $mtr -zwnr4T -P $port -c 100 $ip 2>&1 | tee -a ~/sanity_test_results/mtr_data_$(hostname)_${ip}_${port}_tcp_1_100_$(date -u +"%m%d%H%M").txt
    done
done
exec bash    