#!/bin/bash
chmod 400 ~/.ssh/terran_key
outd=/data/pzhu/st-data/$(hostname)
ssh pzhu011@terran mkdir -p $outd
while true; do
    rsync -avzS --progress --remove-source-files ~/sanity_test/rs/* pzhu011@terran.cs.ucr.edu:$outd/
    sleep 3600
done