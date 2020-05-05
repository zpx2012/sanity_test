#! /bin/bash

cd ~/sanity_test/triple_ack
bash install.sh
make

url="https://13.229.189.0/scipy/scipy/zip/master"
stime=$(date -u +'%Y%m%d%H%M')
screen -dmS curl_ctl bash -c "while true;do bash ../curl_dler_url.sh $url github-tripleack 120 https $stime 6000;done"
sleep 120
screen -dmS curl_ack bash -c "while true;do bash ../curl_dler_url.sh $url github-tripleack 120 https $stime 5000;done"
screen -dmS triple_ack ./triple_ack 13.229.189.0 5000