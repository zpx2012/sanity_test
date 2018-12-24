startime=$(date -u +"%m%d%H%M")utc
while true
do 
sudo /usr/local/sbin/mtr -zwnr4 -c 100 $1 2>&1 | tee -a ~/sanity_test_results/mtr_old_$(hostname)2$2_icmp_1_100_$startime.txt
done
exec bash