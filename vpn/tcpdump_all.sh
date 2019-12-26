#!/bin/bash
ip=$1
port=$2
hm=$3
inf=`ip route get 8.8.8.8 | head -n1 | awk -- '{print $5x}'`
outdir=~/sanity_test/rs/tcpdump_$(date -u +%m%d%H%M)
mkdir $outdir
cd $outdir
sudo tcpdump -w tcpdump_${hm}_$(hostname)_"%m%d%H%M%S%z".pcap -G 1 -s 96 -i $inf 