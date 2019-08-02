dir=~/sanity_test/cernet
out=$dir/$(hostname)_cernet_$(date -u +%m%d%H%M).txt
cat $dir/10ip_perAS.csv | while IFS=',' read ip school;do
    echo $ip,$school >> $out
    mtr -zwnr4 -c 60 $ip 2>&1 | tee -a $out
    echo >> $out
    echo >> $out
done