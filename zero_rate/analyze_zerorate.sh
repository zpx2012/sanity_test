for key in intl cn; do
	for f in *${key}*.csv;
	do
		echo $f
		cat $f | wc -l
		echo browser
		awk -F "\"*,\"*" '{print $3}' $f | grep False | wc -l
		echo Urllib2
		awk -F "\"*,\"*" '{print $4}' $f | grep 'timeout\|reset\|urlerror' | wc -l
		echo Ping
		awk -F "\"*,\"*" '{print $5}' $f | grep True | wc -l
		awk -F "\"*,\"*" '{print $2,$5}' $f | grep True | grep github | wc -l
		echo 'False, timeout, True'
		cat $f | grep 'False,timeout,True' | wc -l
		cat $f | grep 'False,timeout,True' 
		echo 'False, reset, True'
		cat $f | grep 'False,reset,True' | wc -l
		cat $f | grep 'False,reset,True'
		echo 'False, urlerror, True'
		cat $f | grep 'False,urlerror,True' | wc -l
		cat $f | grep 'False,urlerror,True'
	done
done