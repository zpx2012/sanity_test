#!/bin/bash
ip=$1
port=$2
hm=$3
role=$4
start=$5
inf=`ip route get 8.8.8.8 | head -n1 | awk -- '{print $5x}'`
outdir=~/sanity_test/rs/tcpdump_$(echo $start | cut -c1-4)
outfile=tcpdump_${hm}_$(hostname)_${start}_client.pcap
if [[ $role == s ]]; then
    outfile=tcpdump_$(hostname)_${hm}_${start}_server.pcap
fi
mkdir $outdir
cd $outdir
sudo tcpdump -w $outfile -vv -W 2000 -C 1024 -s 96 -i $inf -n host $ip and tcp port $port