f=$1
n=$2
i=0
trfile=~/sanity_test/rs/$(echo ${f/pscan/tr}| sed -e 's/.txt//g')_$(hostname)_$(date -u +"%m%d%H%M").txt
cat $f | while IFS=' ' read closed tcp port ip ts; do
    if ((i < 5));then
        sudo traceroute -A --sport=$n -p $port -T -f 4 -m 25 $ip > otr_$ip
        cat otr_$ip
        cat otr_$ip >> $trfile
        rt=`cat otr_$ip | grep -e '202\.97\.\|AS4134'`
        if [ ! -z "$rt" -a "$rt" != " " ]; then
            ((i++))
            echo $ip $port >> ~/sanity_test/rs/via4134_$(hostname).txt
        fi
    fi
done