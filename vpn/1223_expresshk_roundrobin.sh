stime=$(date -u +'%Y%m%d%H%M%S')
dur=50
mkdir -p ~/sanity_test/rs
while true;do
    screen -dmS http bash ~/sanity_test/curl_dler.sh 3.86.202.146 VG-AWS-VPN $dur http $stime
    echo HTTP starts
    screen -ls
    sleep $dur
    sleep 5
    screen -S http -X quit 
    echo HTTP ends
    echo
    screen -dmS vpn openvpn --config ~/hk4-expressvpn.ovpn
    sleep 20
    echo VPN starts
    screen -ls
    echo -----------------------
    ip route
    echo -----------------------
    screen -dmS vpnhttp bash ~/sanity_test/curl_dler.sh 3.86.202.146 VG-AWS-VPN $dur vpn $stime
    sleep $dur
    sleep 5
    screen -S vpn -X quit
    screen -S vpnhttp -X quit
    echo VPN ends
    echo 
done