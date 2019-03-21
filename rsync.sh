#!/bin/bash
<<<<<<< HEAD
chmod 400 ~/.ssh/terran_key
outd=/data/pzhu/st-data/$(hostname)
=======
outd=/data/pzhu/st-data/$(hostname)
mv ~/sanity_test/ip_scan/data/terran ~/.ssh/
>>>>>>> 4a3c95d53e9ed6fe94ab78173b61943bb0f2760a
ssh pzhu011@terran mkdir -p $outd
while true; do
    rsync -avzS --progress --remove-source-files ~/sanity_test/rs/* pzhu011@terran.cs.ucr.edu:$outd/
    sleep 3600
done