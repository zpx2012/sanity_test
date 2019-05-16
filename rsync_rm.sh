#!/bin/bash
chmod 400 ~/.ssh/id_rsa
outd=/data/pzhu/st-data/$(hostname)
ssh -oStrictHostKeyChecking=no pzhu011@terran.cs.ucr.edu mkdir -p $outd
while true; do
    rsync -avzS --progress --remove-source-files ~/sanity_test/rs/* pzhu011@terran.cs.ucr.edu:$outd/
    sleep 3600
done