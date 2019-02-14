#!/bin/bash
cd ~/sanity_test/ip_scan
gcc open_thrput.c -o open_thrput.o
cat ~/sanity_test/ip_scan/data/0214_thrput.csv | while IFS=',' read ip cp op sp fl vp;do
if [[ $(hostname) == $vp ]];then
    thrput_ofile=~/sanity_test_results/opthrput_${ip}_${op}_${sp}_$(hostname)_$(date -u +%m%d%H%M).txt
    screen -dmS hping3_ptr_$ip bash ~/sanity_test/ip_scan/hping3_ptr.sh $ip $cp $((sp+1)) $fl 1 60 $ip $op $sp 
    screen -dmS thrput_$ip bash -c "while true;do ./open_thrput.o $ip $op $sp >> $thrput_ofile;done"
fi

    # sudo traceroute -A --sport=$n -p $port -T -f 4 -m 25 $ip > otr
    # rt=`cat otr | grep -e '202\.97\.\|AS4134'`
    # if [ ! -z "$rt" -a "$rt" != " " ]; then
