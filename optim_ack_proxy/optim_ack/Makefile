
all:
	g++ -Wno-write-strings -DSUBCONN_NUM=$(subconn_num) -I/usr/include test.c sendicmp.c hping2.c sendtcp.c waitpacket.c util.c logging.c socket.c thr_pool.c -o test -lnetfilter_queue -lnfnetlink  -pthread

debug:
	g++ -Wno-write-strings -DSUBCONN_NUM=$(subconn_num) -g -I/usr/include test.c sendicmp.c hping2.c sendtcp.c waitpacket.c util.c logging.c socket.c thr_pool.c -o test -lnetfilter_queue -lnfnetlink  -pthread

clean:
	rm *.o
	rm test


