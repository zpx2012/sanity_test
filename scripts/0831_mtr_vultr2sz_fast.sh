startime=$(date +"%m%d%H%M")
while true
do 
sudo ~/mtr-modified/mtr -zwnr4T --port 80 -i 0.01 -c 1000 39.108.98.242 2>&1 | tee -a ~/sanity_test_results/mtr_new_$(hostname)2sz_tcp_0.01_1000_$startime.txt;
done
exec bash