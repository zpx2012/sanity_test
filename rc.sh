#!/bin/bash
if [ -z "$1" ]
  then
    echo "No argument supplied"
    exit 1
fi
screen -dmS o2c bash ~/sanity_test/o2c/0319_cn.sh $1