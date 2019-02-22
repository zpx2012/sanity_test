#!/bin/bash
ip=$1
port=$2
hm=$3
inf=`ip route get 8.8.8.8 | head -n1 | awk -- '{print $5x}'`
outdir=~/sanity_test_results/tcpdump_$(date -u +%m%d)
mkdir $outdir
cd $outdir
sudo tcpdump -w tcpdump_${hm}_$(hostname)_$(date -u +%m%d%H%M).pcap -W 2000 -C 1024 -s 96 -i $inf -n host $ip and tcp port $port