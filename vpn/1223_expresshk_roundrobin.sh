ip=$1
hn=$2
dur=$3
lp=5000
stime=$(date -u +'%Y%m%d%H%M')
mkdir -p ~/sanity_test/rs
while true;do
    screen -dmS http bash ~/sanity_test/curl_dler.sh $ip $hn $dur http $stime $lp
    echo HTTP starts
    screen -ls
    echo -----------------------
    ip route
    echo -----------------------
    sleep $dur
    sleep 5
    screen -S http -X quit 
    echo HTTP ends

    echo
    screen -dmS vpn openvpn --config ~/sanity_test/vpn/hk4-expressvpn.ovpn
    sleep 20
    echo VPN starts
    screen -ls
    echo -----------------------
    ip route
    echo -----------------------
    screen -dmS vpnhttp bash ~/sanity_test/curl_dler.sh $ip $hn $dur vpn $stime $lp
    sleep $dur
    sleep 5
    sudo killall openvpn
    screen -S vpnhttp -X quit
    echo VPN ends
    echo 
done