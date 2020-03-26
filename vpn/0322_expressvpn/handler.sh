#! /bin/bash
dir_name=0322_expressvpn
vpnfile=~/sanity_test/vpn/0322_expressvpn/expressvpn_wMtr.sh

stime=$(date -u +'%Y%m%d%H%M')
log=~/sanity_test/rs/screenlog_${dir_name}_$(hostname)_$stime
mkdir -p ~/sanity_test/rs
cd ~/sanity_test/vpn/$dir_name
while true;do
    cat data/$(hostname).csv | while IFS=',' read ip hn lp; do
        bash $vpnfile $ip $hn 30 $((lp+1)) $stime $log hk
        bash $vpnfile $ip $hn 30 $((lp+3)) $stime $log jp
        bash $vpnfile $ip $hn 30 $((lp+4)) $stime $log us
		bash ~/sanity_test/curl_dler.sh $ip $hn 30 http $stime $lp
    done
done

#Notes:
# python run_cmd_over_ssh.py 0323c_express.csv 'cd sanity_test;git pull;screen -dmS expressvpn bash vpn/0322_expressvpn/handler.sh' -ch -s
# make sure the hostname is changed, interval between each vps
# screen -S expressvpn -X quit can not kill the process
# need to run ps -ef| grep express to check
