#! /bin/bash

ip=$1
hn=$2
dur=$3
lp=$4
stime=$5
log=$6
country=$7
con_log=~/expressvpn_single_${hn}_${country}_$(date +%s)
connectivity_log=~/sanity_test/rs/succrate_$(hostname)_expressvpn_${stime}.csv
start_time=0

echo For $hn $ip $dur $lp $country | tee -a $log
for i in 1 2 3 4 5;do
    start_time=$(date -u --rfc-3339=seconds)
    echo $(date +%s)": Try" $i  | tee -a $log
    screen -dmS vpn_${hn}_$country bash -c "expressvpn connect $country > $con_log"
    for j in {1..30};do
        if sudo cat $con_log | grep -q 'Connected to'; 
        then
            break
        fi
        # echo sleep 1 sec | tee -a $log
        sleep 1
    done
    if sudo cat $con_log | grep -q 'Connected to'; 
    then
        echo $start_time, $(date -u --rfc-3339=seconds), country, Success >> $connectivity_log 
        sudo cat $con_log >> $log
        echo $(date +%s)": VPN starts"  | tee -a $log
        echo ----------------------- | tee -a $log
        ip route | tee -a $log
        echo ----------------------- | tee -a $log
        screen -dmS mtr_${hn}_${country}_$(date +%s) bash ~/sanity_test/vpn/mtr-ins.sh mtr $(curl ifconfig.io) $hn 1195 60 0.5
        bash ~/sanity_test/curl_dler.sh $ip $hn $dur expressvpn-$country $stime $lp
        expressvpn disconnect
        echo $(date +%s)":VPN ends" | tee -a $log
        break
    else
        echo $start_time, $(date -u --rfc-3339=seconds), country, Fail >> $connectivity_log
    fi
    screen -S vpn_${hn}_$country -X quit
    sudo killall expressvpn
    sudo rm $con_log
done

