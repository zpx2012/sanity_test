#!/bin/bash
chmod 400 ~/.ssh/id_rsa
outd=/data/pzhu/st-data/$(hostname)
ssh -oStrictHostKeyChecking=no pzhu011@terran.cs.ucr.edu mkdir -p $outd
while true; do
    rsync -avzS --remove-source-files --progress ~/sanity_test/rs/tcpdump_* pzhu011@terran.cs.ucr.edu:$outd/
    sleep 3600
done