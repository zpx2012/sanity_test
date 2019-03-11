startime=$(date +"%m%d%H%M")
while true
do 
sudo mtr -zwnr4T -c 60 169.235.31.181 2>&1 | tee -a ~/sanity_test/rs/mtr_old_$(hostname)2terran_syn_1_10_$startime.txt
done
exec bash