#!/bin/bash
# if [ -z "$1" ]
#   then
#     echo "No argument supplied"
#     exit 1
# fi
sudo apt-get update
sudo apt-get install -y git screen
cd ~
screen -dmS run bash -c 'wget http://101.133.171.40/sa.tar.gz;tar -xzvf sa.tar.gz;cd sanity_test;bash install.sh'
# screen -dmS run bash -c 'git clone --recurse-submodules https://github.com/zpx2012/sanity_test.git;cd sanity_test;bash install.sh'
# screen -dmS o2c bash o2c/0319_cn.sh $1
# screen -dmS rsync bash rsync.sh