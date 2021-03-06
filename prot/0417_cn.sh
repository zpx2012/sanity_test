#!/bin/bash
cd ~/sanity_test/prot
mtr=~/sanity_test/mtr-insertion/mtr
for day in 0 1;do
    cat data/$(hostname)_day${day}.csv | while IFS=' ' read ip hn http_sp https_sp ss_lp ss_sp iperf_dp iperf_sp; do
        screen -dmS td_$hn bash tcpdump_iponly.sh $ip $hn
        screen -dmS ss_$hn sslocal -c ../shadowsocks/${hn}_client.json
        screen -dmS poller_$hn bash poller.sh $ip $hn $http_sp $https_sp $ss_lp $ss_sp $iperf_dp $iperf_sp
        screen -dmS mtr_${hn}_http bash mtr.sh $ip $hn 80 $http_sp 60 $mtr http
        screen -dmS mtr_${hn}_ssl bash mtr.sh $ip $hn 443 $https_sp 60 $mtr https
        screen -dmS mtr_${hn}_ss bash mtr.sh $ip $hn 8388 $ss_lp 60 $mtr ss
        screen -dmS mtr_${hn}_iperf bash mtr.sh $ip $hn $iperf_dp $iperf_sp 60 $mtr iperf3
    done
    sleep 86400
    bash ~/sanity_test/ks.sh td
    bash ~/sanity_test/ks.sh ss
    bash ~/sanity_test/ks.sh poller
    bash ~/sanity_test/ks.sh mtr
done

cd ~/sanity_test;./ks.sh mtr;./ks.sh ss;./ks.sh poller;./ks.sh prot;./ks.sh td;git pull;screen -dmS prot bash ~/sanity_test/prot/0417_cn.sh;sleep 4;screen -x