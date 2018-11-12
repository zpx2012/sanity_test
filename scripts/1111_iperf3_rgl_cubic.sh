while true 
do 
startime=$(date -u +"%m%d%H%Mutc")
iperf3 -c 169.235.31.181 -p 20000 -b 1M -f K -t 5 -4VRd -C cubic --logfile ~/iperf3_$USER_$(hostname)_terran_cubic_$startime.log 
sleep 10
done