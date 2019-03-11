startime=$(date +"%m%d%H%M")
while true
do 
mtr -zwnr4T -c 60 128.112.18.21 2>&1 | tee -a ~/sanity_test/rs/mtr_old_$(hostname)2princeton_syn_1_10_$startime.txt
done
exec bash