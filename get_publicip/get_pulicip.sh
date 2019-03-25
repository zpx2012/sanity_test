cd ~/sanity_test/get_publicip
cat 0309_other.csv | while IFS=',' read k u ip n; do
    nc -zv -p 10000 $ip 443
done