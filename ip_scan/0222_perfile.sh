f=$1
n=$2
i=0
trfile=~/sanity_test_results/$(echo ${f/pscan/tr}| sed -e 's/.txt//g')_$(hostname)_$(date -u +"%m%d%H%M").txt
cat $f | while IFS=' ' read closed tcp port ip ts; do
    if ((i < 5));then
        sudo traceroute -A --sport=$n -p $port -T -f 4 -m 25 $ip > otr_$ip
        cat otr_$ip
        cat otr_$ip >> $trfile
        rt=`cat otr_$ip | grep -e '202\.97\.\|AS4134' | wc -l`
        # if [ ! -z "$rt" -a "$rt" != " " ]; then
        if (( $rt > 2 ));then
            ((i++))
            echo $ip $port >> via4134.txt
            screen -dmS td_$ip bash ~/sanity_test/ip_scan/tcpdump_whole.sh $ip $port $ip
            screen -dmS opt_$ip bash -c "while true;do ./open_thrput.o $ip $op $sp;done"
            screen -dmS mtr_$ip bash ~/sanity_test/scripts/tmpl_mtr_tcp_1_100.sh $ip $port ~/mtr-modified/mtr

        fi
    fi
done