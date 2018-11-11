while true 
do 
iperf3 -c 169.235.31.181 -p 20000 -b 1M -f k -t 5 -4VRd --logfile ~/iperf3_$USER_$(hostname)_terran_cubic_$(date -u +"%m%d%H%M").log 
sleep 10
done