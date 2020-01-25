#! /bin/bash
#Country Code: HK, TW, JP, US

ip=$1
hn=$2
dur=$3
lp=$4
stime=$5
log=$6
country=$7
con_log=~/purevpn_output

for i in 1 2 3 4 5 6 7 8 9 10;do
    echo "Try" $i | tee -a $log
    date -u +'%Y%m%d%H%M' | tee -a $log
    purevpn -c "$country" | tee $con_log
    if cat $con_log | grep -q '*** VPN Connected ***'; then
        date -u +'%Y%m%d%H%M' | tee -a $log
        echo VPN starts | tee -a $log
        screen -ls | tee -a $log
        echo ----------------------- | tee -a $log
        ip route | tee -a $log
        echo ----------------------- | tee -a $log
        bash ~/sanity_test/curl_dler.sh $ip $hn $dur purevpn_$country $stime $lp
        purevpn -d
        echo VPN ends | tee -a $log
        echo | tee -a $log
        break
    fi
done