#!/bin/bash
port=$1
inf=`ip route get 8.8.8.8 | head -n1 | awk -- '{print $5x}'`
outdir=~/sanity_test/rs/tcpdump_$(date -u +%m%d)
outfile=tcpdump_$(hostname)_$(date -u +%m%d%H%M)_server.pcap
mkdir $outdir
cd $outdir
sudo tcpdump -w $outfile -vv -W 2000 -C 1024 -s 96 -i $inf -n tcp port $port