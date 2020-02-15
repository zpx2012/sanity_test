cat ~/sanity_test/vpn/data/$1/$(hostname).csv | while IFS=',' read ip hn lp vpn vpn_ip; do
    bash ~/sanity_test/vpn/mtr-ins.sh mtr ${ip} $hn 80 60 0.5 $(date -u %Y%m%d%H%M%S)
done
exec bash