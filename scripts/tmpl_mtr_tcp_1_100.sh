startime=$(date -u +"%m%d%H%M")utc
while true
do 
sudo mtr -zwnr4T -P $2 -c 100 $1 2>&1 | tee -a ~/sanity_test_results/mtr_old_$(hostname)_$1_tcp_1_100_$startime.txt
done
exec bash