startime=$(date +"%m%d%H%M")
while true
do 
sudo ~/sanity_test/mtr-modified-1.0/mtr -zwnr4T --port 80 -i 0.01 -c 1000 39.108.98.242 2>&1 | tee -a ~/sanity_test/rs/mtr_new_$(hostname)2sz_tcp_0.01_1000_$startime.txt;
done
exec bash