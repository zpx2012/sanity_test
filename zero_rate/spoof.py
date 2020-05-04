#!/usr/bin/python
from scapy.all import *

ip=IP(src="39.100.107.202", dst="54.186.76.200")
TCP_SYN=TCP(sport=1500, dport=80, flags="S", seq=100)

# TCP_SYNACK=sr1(ip/TCP_SYN)

# my_ack = TCP_SYNACK.seq + 1
# TCP_ACK=TCP(sport=1500, dport=80, flags="A", seq=101, ack=2000)
# send(ip/TCP_ACK)

my_payload='''GET /ultrasurf HTTP/1.1
Host: 54.186.76.200
Connection: keep-alive
Upgrade-Insecure-Requests: 1
User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_14_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/80.0.3987.163 Safari/537.36
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
Accept-Encoding: gzip, deflate
Accept-Language: zh-CN,zh;q=0.9,en;q=0.8
'''
TCP_PUSH=TCP(sport=1500, dport=80, flags="PA", seq=101, ack=2000)

send(ip/TCP_SYN)
# send(ip/TCP_ACK)
send(ip/TCP_PUSH/my_payload)

