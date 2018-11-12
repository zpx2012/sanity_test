#!/bin/sh
while true 
do 
startime=$(date -u +"%m%d%H%Mutc")
if [ $3 == regular ]
then
iperf3 -c $1 -p $2 -b 1M -f K -t 5 -4Vd -C cubic --logfile ~/iperf3_$(hostname)_$3_$1_cubic_$startime.txt 
else
sudo iperf3 -c $1 -p $2 -b 1M -f K -t 5 -4Vd --logfile ~/iperf3_$(hostname)_$3_$1_$startime.txt
fi
sleep 10
done