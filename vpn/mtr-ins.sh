#! /bin/bash

mtr=$1
ip=$2
hn=$3
dp=$4
cnt=$5
intvl=$6
stime=$7

name=$(basename "$(dirname "$mtr")")
if [ $name == '.' ]; then 
    name=mtr
fi

single_mtr=singlerun_${hn}_${name}_$(date -u %Y%m%d%H%M%S)
out=~/sanity_test/rs/${name}_$(hostname)_${hn}_tcp_${intvl}_${cnt}_${stime}.txt

for i in 1 2 3 4 5 6 7 8 9 10;do
    echo $ip $hn $dp | tee -a $out
    sudo $mtr -zwnr4 -P $dp -c $cnt -i $intvl ${ip} 2>&1 | tee $single_mtr
    if ! cat $single_mtr | grep -q 'send_inserted_tcp_packet:time out'; then
        cat $single_mtr >> $out
        echo >> $out
        rm $single_mtr
        break
    fi
done
