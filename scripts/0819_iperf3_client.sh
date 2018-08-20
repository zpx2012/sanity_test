while true 
do 
iperf3 -c 169.235.31.181 -p 80 -b 1M -f k -t 5 -4 --logfile ~/iperf3.log 
sleep 10
done