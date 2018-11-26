screen -dmS ss ssserver -c ~/sanity_test/shadowsocks/ss_server.json
sleep 120
screen -dmS sched python3 ~/sanity_test/sched.py