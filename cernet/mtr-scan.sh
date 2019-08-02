dir=~/sanity_test/cernet
out=$dir/$(hostname)_cernet.txt
cat $dir/10ip_perAS.csv | while IFS=',' read ip school;do
    mtr -zwnr4 -c 60 $ip 2>&1 | tee -a $out
done