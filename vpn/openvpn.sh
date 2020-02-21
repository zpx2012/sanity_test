#! /bin/bash

ip=$1
hn=$2
dur=$3
lp=$4
stime=$5
log=$6
ovpn=$7
ovpn_base=$(basename $ovpn)
con_log=~/openvpn_single_${hn}_$(date +%s)

echo log:$log, ovpn:$ovpn

for i in 1 2 3 4 5;do
    echo $(date +%s)": Try" $i  | tee -a $log
    screen -dmS vpn_${hn} bash -c "sudo openvpn --config $ovpn --log $con_log;exec bash"
    for j in {1..30};do
        if cat $con_log | grep -q 'Initialization Sequence Completed'; 
        then
            break
        fi
        echo sleep 1 sec | tee -a $log
        sleep 1
    done
    if cat $con_log | grep -q 'Initialization Sequence Completed'; 
    then
        cat $con_log | tee -a $log
        echo $(date +%s)": VPN starts"  | tee -a $log
        echo ----------------------- | tee -a $log
        ip route | tee -a $log
        echo ----------------------- | tee -a $log
        bash ~/sanity_test/curl_dler.sh $ip $hn $dur ${ovpn_base%.*} $stime $lp
        screen -S vpn_$hn -X quit
        echo $(date +%s)":VPN ends" | tee -a $log
        break
    else
        screen -S vpn_$hn -X quit
    fi
done