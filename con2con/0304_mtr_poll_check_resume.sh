f=$1
fm=${f}_m
mtr=$2
start=$3
i=15
while true;do
    if (( i == 15 ));then #check
        cp /dev/null $fm
        cat $f | while IFS=' ' read ip hn dp sp; do
            echo $ip $hn $dp $sp
            sudo $mtr -zwnr4T -P $dp -i 0.1 -c 20 $ip 2>&1 | tee tmp
            max=`cat tmp | tail -n1 | sed 's@^[^0-9]*\([0-9]\+\).*@\1@'`
            if (( max < 10 ));then
                let max=19
            fi
            echo $ip $hn $dp $sp $((max+1)) >> $fm
            cat tmp >> ~/sanity_test/rs/mtr_data_$(hostname)_${hn}_${sp}_tcp_0.1_50_check_${start}.txt
        done
        let i=0    
    fi
    cat $fm | while IFS=' ' read ip hn dp sp max; do
        echo $ip $hn $dp $sp
        sudo $mtr -zwnr4T -m $max -P $dp -c 100 $ip 2>&1 | tee -a ~/sanity_test/rs/mtr_data_$(hostname)_${hn}_${sp}_tcp_1_100_${start}.txt
    done
    let i=i+1
done
exec bash    