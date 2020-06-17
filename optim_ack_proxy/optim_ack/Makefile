
all:
	g++ -I/usr/include test.c sendicmp.c hping2.c sendtcp.c waitpacket.c util.c logging.c socket.c -o test -lnetfilter_queue -lnfnetlink  -pthread

debug:
	g++ -g -I/usr/include test.c sendicmp.c hping2.c sendtcp.c waitpacket.c util.c logging.c socket.c -o test -lnetfilter_queue -lnfnetlink  -pthread

clean:
	rm *.o
	rm test


