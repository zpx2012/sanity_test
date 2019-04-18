#!/bin/bash
inf=`ip route get 8.8.8.8 | head -n1 | awk -- '{print $5x}'`
outdir=~/sanity_test/rs/tcpdump_$(date -u +%m%d)
mkdir $outdir
outfile=tcpdump_$(hostname)_$(date -u +%m%d%H%M)_server.pcap
sudo tcpdump -w $outdir/$outfile -vv -W 2000 -C 1024 -s 96 -i $inf -n tcp port 80 or tcp port 443 or tcp port 8388 or tcp port 5201 or tcp port 5202 or tcp port 5203 or tcp port 5204 or tcp port 5205 or tcp port 5206 or tcp port 5207 or tcp port 5208
