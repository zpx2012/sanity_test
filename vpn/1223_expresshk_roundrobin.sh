stime=$(date -u +'%Y%m%d%H%M%S')
mkdir -p ~/sanity_test/rs
while true;do
    screen -dmS http bash ~/sanity_test/curl_dler.sh 3.86.202.146 VG-AWS-VPN 3000 http $stime
    screen -S http -X quit 
    sleep 3005
    screen -dmS vpn openvpn --config hk4-expressvpn.ovpn
    sleep 10
    screen -dmS http bash ~/sanity_test/curl_dler.sh 3.86.202.146 VG-AWS-VPN 3000 vpn $stime
    sleep 3005
    screen -S vpn -X quit
    screen -S http -X quit
done