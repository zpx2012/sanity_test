ip=$1
port=$2
n=$3
trfile=$4

# sudo paris-traceroute -Q -s $n -d $port -p tcp -f 4 -m 25 $ip > optr 2>&1
# cat optr
# cat optr >> ~/sanity_test/rs/ptr_redo0124_$(hostname)_$(date -u +"%m%d%H%M").txt
# screen -dmS tr_$ip bash ~/sanity_test/ip_scan/tr.sh $ip $port $n
sudo traceroute -A --sport=$n -p $port -T -f 4 -m 25 $ip > otr_$n
cat otr_$n
cat otr_$n >> $trfile
rt=`cat otr_$n | grep -e '202\.97\.\|AS4134'`
if [ ! -z "$rt" -a "$rt" != " " ]; then
    hping3 -SA -i u500000 -c 10 -s $n -p $port $ip 2> oncSA_$n;cat oncSA_$n
    hping3 -S -i u500000 -c 10 -s $n -p $port $ip 2> oncS_$n;cat oncS_$n
    # rt=`cat onc | grep '100% packet loss'`
    if ! cat oncSA_$n | grep -q '100% packet loss'; then            
        echo SYNACK: $ip $port  
        screen -dmS hping3_SA_$ip bash ~/sanity_test/ip_scan/hping3.sh $ip $port $n SA u500000 120
        # sleep 20

    elif ! cat oncS_$n | grep -q '100% packet loss'; then
        echo SYN: $ip $port  
        screen -dmS hping3_S_$ip bash ~/sanity_test/ip_scan/hping3.sh $ip $port $n S 1 60
        # sleep 15
        # screen -dmS ptr_$ip bash ~/sanity_test/ip_scan/ptr.sh $ip $port $n
    else
        echo $ip $port >> via4134.txt
    fi
fi