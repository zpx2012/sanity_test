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
for i in 1 ;do
    start_time=$(date -u --rfc-3339=seconds)
    start_tstamp=$(date +%s)
    echo $(date +%s)": Try" $i  | tee -a $log
    screen -dmS vpn_${hn}_$country bash -c "expressvpn connect $country > $con_log"
    sleep 1
    for j in {1..119};do
        if cat $con_log | grep -q 'Connected to'; 
        then
            break
        fi
        sleep 1
    done
    cat $con_log
    if cat $con_log | grep -q 'Connected to'; 
    then
        end_tstamp=$(date +%s)
        echo $start_time, $((end_tstamp-start_tstamp)), $country, Success >> $connectivity_log 
        cat $con_log >> $log
        echo $(date +%s)": VPN starts"  | tee -a $log
        echo ----------------------- | tee -a $log
        ip route | tee -a $log
        echo ----------------------- | tee -a $log
        vpn_ip=$(curl ifconfig.io)
        bash ~/sanity_test/curl_dler.sh $ip $hn $dur expressvpn-$country $stime $lp
        expressvpn disconnect
        echo $(date +%s)":VPN ends" | tee -a $log
        rm $con_log
        screen -dmS mtr_${hn}_${country}_$(date +%s) bash ~/sanity_test/vpn/mtr-ins.sh mtr $vpn_ip expressvpn_$country 1195 60 0.5 $stime
        break
    else
        echo $start_time, 0, $country, Fail >> $connectivity_log
    fi
    expressvpn disconnect
    screen -S vpn_${hn}_$country -X quit
    sudo killall expressvpn
    rm $con_log
done

