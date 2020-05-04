#!/usr/bin/env python
  
import os
import random
import time

from scapy.all import send, sr1, sr, TCP, IP, Raw

#SERVER_IP = '103.235.46.40'
#SERVER_IP = '169.235.31.181'
SRC_IP = '75.142.207.100'
SERVER_IP = '47.113.86.254'
SERVER_PORT = 80

HTTP_REQ = 'GET /ultrasurf HTTP/1.1\r\nHost: www.kankan.com\r\n\r\n'

def spoof(http_req):
    client_port = random.randint(10000, 60000)
    client_ISN = random.getrandbits(32)
    server_ISN = random.getrandbits(32)

    syn_pkt = IP(src=SRC_IP, dst=SERVER_IP)/TCP(sport=client_port, dport=SERVER_PORT, flags='S', seq=client_ISN)
    send(syn_pkt)
#syn_ack_pkt = sr1(syn_pkt)
#server_ISN = syn_ack_pkt[TCP].seq

    time.sleep(1)

#ack_pkt = IP(dst=SERVER_IP)/TCP(sport=client_port, dport=SERVER_PORT, flags='A', seq=client_ISN + 1, ack=server_ISN + 1)
#send(ack_pkt)

    req_pkt = IP(src=SRC_IP, dst=SERVER_IP)/TCP(sport=client_port, dport=SERVER_PORT, flags='A', seq=client_ISN + 1, ack=server_ISN + 1)/Raw(load=http_req)
    send(req_pkt)

spoof(HTTP_REQ)
time.sleep(3)
spoof('GET my HTTP/1.1\r\nHost: www.kankan.com\r\n\r\n')
time.sleep(3)
spoof('GET my HTTP/1.1\r\nHost: www.kankan.com\r\n\r\n')
