#! /bin/bash
#Country Code:hk, tw, jp, us

ip=$1
hn=$2
dur=$3
lp=$4
stime=$5
log=$6
country=$7
con_log=~/nordvpn_output

for i in 1 2 3 4 5 6 7 8 9 10;do
    echo "Try" $i | tee -a $log
    date -u +'%Y%m%d%H%M' | tee -a $log
    nordvpn c "$country" | tee $con_log
    if cat $con_log | grep -q 'You are connected'; then
        date -u +'%Y%m%d%H%M' | tee -a $log
        echo VPN starts | tee -a $log
        screen -ls | tee -a $log
        echo ----------------------- | tee -a $log
        ip route | tee -a $log
        echo ----------------------- | tee -a $log
        bash ~/sanity_test/curl_dler.sh $ip $hn $dur nordvpn_$country $stime $lp
        nordvpn disconnect
        echo VPN ends | tee -a $log
        echo | tee -a $log
        break
    fi
done