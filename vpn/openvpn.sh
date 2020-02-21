#! /bin/bash

ip=$1
hn=$2
dur=$3
lp=$4
stime=$5
log=$6
ovpn=$7
con_log=~/openvpn_single_${hn}_$(date -u +'%Y%m%d%H%M%S')

for i in 1 2 3 4 5 6 7 8 9 10;do
    echo $(date +%s)": Try" $i  | tee -a $log
    screen -dmS vpn sudo openvpn --config ~/sanity_test/vpn/$ovpn --log $con_log
    for j in 1 2 3 4 5;do
        if cat $con_log | grep -q 'Initialization Sequence Completed'; then
            break
        fi
        echo sleep 1 sec | tee -a $log
        sleep 1
    done
    if cat $con_log | grep -q 'Initialization Sequence Completed'; then
        cat $con_log | tee -a $log
        echo $(date +%s)": VPN starts"  | tee -a $log
        echo ----------------------- | tee -a $log
        ip route | tee -a $log
        echo ----------------------- | tee -a $log
        bash ~/sanity_test/curl_dler.sh $ip $hn $dur ${ovpn%.*} $stime $lp
        sudo killall openvpn
        echo $(date +%s)":VPN ends" | tee -a $log
        break
    fi
done