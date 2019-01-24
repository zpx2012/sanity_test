#include <linux/tcp.h>
#include <linux/ip.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <signal.h>

uint32_t seq = 0;
uint32_t ack_seq = 0;
int raw_sock_tx = 0;
int raw_sock_rx = 0;
unsigned int msg_len = 1400;
uint16_t seed;
struct sockaddr_in server, client;

char pos_str[] = "\x48\x54\x54\x50\x2f\x31\x2e\x31\x20\x32\x30\x30\x20\x4f \
\x4b\x0d\x0a\x73\x65\x72\x76\x65\x72\x3a\x20\x65\x63\x73\x74\x61 \
\x74\x69\x63\x2d\x33\x2e\x32\x2e\x31\x0d\x0a\x43\x6f\x6e\x74\x65 \
\x6e\x74\x2d\x54\x79\x70\x65\x3a\x20\x74\x65\x78\x74\x2f\x68\x74 \
\x6d\x6c\x0d\x0a\x65\x74\x61\x67\x3a\x20\x57\x2f\x22\x32\x32\x32 \
\x32\x34\x31\x33\x37\x34\x2d\x34\x30\x39\x36\x2d\x32\x30\x31\x38 \
\x2d\x31\x30\x2d\x31\x38\x54\x31\x31\x3a\x33\x34\x3a\x30\x36\x2e \
\x33\x39\x30\x5a\x22\x0d\x0a\x6c\x61\x73\x74\x2d\x6d\x6f\x64\x69 \
\x66\x69\x65\x64\x3a\x20\x54\x68\x75\x2c\x20\x31\x38\x20\x4f\x63 \
\x74\x20\x32\x30\x31\x38\x20\x31\x31\x3a\x33\x34\x3a\x30\x36\x20 \
\x47\x4d\x54\x0d\x0a\x63\x61\x63\x68\x65\x2d\x63\x6f\x6e\x74\x72 \
\x6f\x6c\x3a\x20\x6d\x61\x78\x2d\x61\x67\x65\x3d\x33\x36\x30\x30 \
\x0d\x0a\x44\x61\x74\x65\x3a\x20\x53\x75\x6e\x2c\x20\x32\x38\x20 \
\x4f\x63\x74\x20\x32\x30\x31\x38\x20\x30\x39\x3a\x30\x36\x3a\x34 \
\x33\x20\x47\x4d\x54\x0d\x0a\x43\x6f\x6e\x6e\x65\x63\x74\x69\x6f \
\x6e\x3a\x20\x6b\x65\x65\x70\x2d\x61\x6c\x69\x76\x65\x0d\x0a\x54 \
\x72\x61\x6e\x73\x66\x65\x72\x2d\x45\x6e\x63\x6f\x64\x69\x6e\x67 \
\x3a\x20\x63\x68\x75\x6e\x6b\x65\x64\x0d\x0a\x0d\x0a\x31\x30\x34 \
\x30\x64\x0d\x0a\x3c\x21\x64\x6f\x63\x74\x79\x70\x65\x20\x68\x74 \
\x6d\x6c\x3e\x0a\x3c\x68\x74\x6d\x6c\x3e\x0a\x20\x20\x3c\x68\x65 \
\x61\x64\x3e\x0a\x20\x20\x20\x20\x3c\x6d\x65\x74\x61\x20\x63\x68 \
\x61\x72\x73\x65\x74\x3d\x22\x75\x74\x66\x2d\x38\x22\x3e\x0a\x20 \
\x20\x20\x20\x3c\x6d\x65\x74\x61\x20\x6e\x61\x6d\x65\x3d\x22\x76 \
\x69\x65\x77\x70\x6f\x72\x74\x22\x20\x63\x6f\x6e\x74\x65\x6e\x74 \
\x3d\x22\x77\x69\x64\x74\x68\x3d\x64\x65\x76\x69\x63\x65\x2d\x77 \
\x69\x64\x74\x68\x22\x3e\x0a\x20\x20\x20\x20\x3c\x74\x69\x74\x6c \
\x65\x3e\x49\x6e\x64\x65\x78\x20\x6f\x66\x20\x2f\x3c\x2f\x74\x69 \
\x74\x6c\x65\x3e\x0a\x20\x20\x20\x20\x3c\x73\x74\x79\x6c\x65\x20 \
\x74\x79\x70\x65\x3d\x22\x74\x65\x78\x74\x2f\x63\x73\x73\x22\x3e \
\x69\x2e\x69\x63\x6f\x6e\x20\x7b\x20\x64\x69\x73\x70\x6c\x61\x79 \
\x3a\x20\x62\x6c\x6f\x63\x6b\x3b\x20\x68\x65\x69\x67\x68\x74\x3a \
\x20\x31\x36\x70\x78\x3b\x20\x77\x69\x64\x74\x68\x3a\x20\x31\x36 \
\x70\x78\x3b\x20\x7d\x0a\x74\x61\x62\x6c\x65\x20\x74\x72\x20\x7b \
\x20\x77\x68\x69\x74\x65\x2d\x73\x70\x61\x63\x65\x3a\x20\x6e\x6f \
\x77\x72\x61\x70\x3b\x20\x7d\x0a\x74\x64\x2e\x70\x65\x72\x6d\x73 \
\x20\x7b\x7d\x0a\x74\x64\x2e\x66\x69\x6c\x65\x2d\x73\x69\x7a\x65 \
\x20\x7b\x20\x74\x65\x78\x74\x2d\x61\x6c\x69\x67\x6e\x3a\x20\x72 \
\x69\x67\x68\x74\x3b\x20\x70\x61\x64\x64\x69\x6e\x67\x2d\x6c\x65 \
\x66\x74\x3a\x20\x31\x65\x6d\x3b\x20\x7d\x0a\x74\x64\x2e\x64\x69 \
\x73\x70\x6c\x61\x79\x2d\x6e\x61\x6d\x65\x20\x7b\x20\x70\x61\x64 \
\x64\x69\x6e\x67\x2d\x6c\x65\x66\x74\x3a\x20\x31\x65\x6d\x3b\x20 \
\x7d\x0a\x69\x2e\x69\x63\x6f\x6e\x2d\x5f\x62\x6c\x61\x6e\x6b\x20 \
\x7b\x0a\x20\x20\x62\x61\x63\x6b\x67\x72\x6f\x75\x6e\x64\x2d\x69 \
\x6d\x61\x67\x65\x3a\x20\x75\x72\x6c\x28\x22\x64\x61\x74\x61\x3a \
\x69\x6d\x61\x67\x65\x2f\x70\x6e\x67\x3b\x62\x61\x73\x65\x36\x34 \
\x2c\x69\x56\x42\x4f\x52\x77\x30\x4b\x47\x67\x6f\x41\x41\x41\x41 \
\x4e\x53\x55\x68\x45\x55\x67\x41\x41\x41\x42\x41\x41\x41\x41\x41 \
\x51\x43\x41\x59\x41\x41\x41\x41\x66\x38\x2f\x39\x68\x41\x41\x41 \
\x41\x47\x58\x52\x46\x57\x48\x52\x54\x62\x32\x5a\x30\x64\x32\x46 \
\x79\x5a\x51\x42\x42\x5a\x47\x39\x69\x5a\x53\x42\x4a\x62\x57\x46 \
\x6e\x5a\x56\x4a\x6c\x59\x57\x52\x35\x63\x63\x6c\x6c\x50\x41\x41 \
\x41\x41\x57\x42\x4a\x52\x45\x46\x55\x65\x4e\x71\x45\x55\x6a\x31 \
\x4c\x78\x45\x41\x51\x6e\x64\x31\x4d\x56\x41\x34\x6c\x79\x49\x45 \
\x57\x78\x36\x55\x49\x4b\x45\x47\x55\x45\x78\x47\x73\x62\x43\x33 \
\x74\x4c\x66\x77\x4a\x2f\x68\x54\x2f\x67\x37\x56\x6c\x43\x6e\x75 \
\x62\x71\x78\x58\x42\x77\x67\x2f\x51\x34\x68\x51\x50\x2f\x4c\x68 \
\x4b\x4c\x35\x6e\x5a\x75\x42\x73\x76\x75\x47\x66\x57\x35\x4d\x47 \
\x79\x75\x7a\x4d\x37\x6a\x7a\x64\x76\x56\x75\x52\x35\x44\x67\x59 \
\x6e\x5a\x2b\x66\x39\x39\x61\x69\x37\x56\x74\x35\x74\x39\x4b\x39 \
\x75\x6e\x75\x34\x48\x4c\x77\x65\x49\x33\x71\x57\x59\x78\x49\x36 \
\x50\x44\x6f\x73\x64\x79\x30\x66\x68\x63\x6e\x74\x78\x4f\x34\x34 \
\x43\x63\x4f\x42\x7a\x50\x41\x37\x6d\x66\x45\x79\x75\x48\x77\x66 \
\x37\x6e\x74\x51\x6b\x34\x6a\x63\x6e\x79\x77\x4f\x78\x49\x6c\x66 \
\x78\x4f\x43\x4e\x59\x61\x4c\x56\x67\x62\x36\x63\x58\x62\x6b\x54 \
\x64\x68\x4a\x58\x71\x32\x53\x49\x6c\x4e\x4d\x43\x30\x78\x49\x71 \
\x68\x48\x63\x7a\x44\x62\x69\x38\x4f\x56\x7a\x70\x4c\x53\x55\x61 \
\x30\x57\x65\x62\x52\x66\x6d\x69\x67\x4c\x48\x71\x6a\x31\x45\x63 \
\x50\x5a\x6e\x77\x66\x37\x67\x62\x44\x49\x72\x59\x56\x52\x79\x45 \
\x69\x6e\x75\x72\x6a\x36\x6a\x54\x42\x48\x79\x49\x37\x70\x71\x56 \
\x72\x46\x51\x71\x45\x62\x74\x36\x54\x45\x6d\x5a\x39\x76\x31\x4e \
\x52\x41\x4a\x4e\x43\x31\x78\x54\x59\x78\x49\x51\x68\x2f\x4d\x6d \
\x52\x55\x6c\x6d\x46\x51\x45\x33\x71\x57\x4f\x57\x31\x6e\x71\x42 \
\x32\x54\x57\x6b\x31\x2f\x33\x74\x67\x4a\x56\x30\x77\x61\x56\x76 \
\x6b\x46\x49\x45\x65\x5a\x62\x48\x71\x34\x45\x6c\x79\x4b\x7a\x41 \
\x6d\x45\x58\x4f\x78\x36\x67\x6e\x45\x56\x4a\x75\x57\x42\x7a\x6d \
\x6b\x52\x4a\x42\x52\x50\x59\x47\x5a\x42\x44\x73\x56\x61\x4f\x6c \
\x70\x53\x67\x56\x4a\x45\x32\x79\x56\x61\x41\x65\x2f\x30\x6b\x78 \
\x2f\x33\x61\x7a\x42\x52\x4f\x30\x56\x73\x62\x4d\x46\x5a\x45\x33 \
\x43\x44\x53\x5a\x4b\x77\x65\x5a\x66\x59\x49\x56\x67\x2b\x44\x5a \
\x36\x76\x37\x68\x39\x47\x44\x56\x4f\x77\x5a\x50\x77\x2f\x50\x6f \
\x78\x4b\x75\x2f\x66\x41\x67\x77\x41\x4c\x62\x44\x41\x58\x66\x37 \
\x44\x64\x51\x6b\x41\x41\x41\x41\x41\x53\x55\x56\x4f\x52\x4b\x35 \
\x43\x59\x49\x49\x3d\x22\x29\x3b\x0a\x7d\x0a\x0a\x69\x2e\x69\x63 \
\x6f\x6e\x2d\x5f\x70\x61\x67\x65\x20\x7b\x0a\x20\x20\x62\x61\x63 \
\x6b\x67\x72\x6f\x75\x6e\x64\x2d\x69\x6d\x61\x67\x65\x3a\x20\x75 \
\x72\x6c\x28\x22\x64\x61\x74\x61\x3a\x69\x6d\x61\x67\x65\x2f\x70 \
\x6e\x67\x3b\x62\x61\x73\x65\x36\x34\x2c\x69\x56\x42\x4f\x52\x77 \
\x30\x4b\x47\x67\x6f\x41\x41\x41\x41\x4e\x53\x55\x68\x45\x55\x67 \
\x41\x41\x41\x42\x41\x41\x41\x41\x41\x51\x43\x41\x59\x41\x41\x41 \
\x41\x66\x38\x2f\x39\x68\x41\x41\x41\x41\x47\x58\x52\x46\x57\x48 \
\x52\x54\x62\x32\x5a\x30\x64\x32\x46\x79";


//Calculate the TCP header checksum of a string (as specified in rfc793)
//Function from http://www.binarytides.com/raw-sockets-c-code-on-linux/
unsigned short csum(unsigned short *ptr,int nbytes) {
    long sum;
    unsigned short oddbyte;
    short answer;

    //Debug info
    //hexdump((unsigned char *) ptr, nbytes);
    //printf("csum nbytes: %d\n", nbytes);
    //printf("csum ptr address: %p\n", ptr);

    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }

    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;

    return(answer);
}


//Pseudo header needed for calculating the TCP header checksum
struct pseudoTCPPacket {
    uint32_t srcAddr;
    uint32_t dstAddr;
    uint8_t zero;
    uint8_t protocol;
    uint16_t TCP_len;
};

int initRawSocket(int protocol) {
    int sock, one = 1;
    //Raw socket without any protocol-header inside
    if((sock = socket(AF_INET, SOCK_RAW, protocol)) < 0) {
        perror("Error while creating socket");
        exit(-1);
    }

    //Set option IP_HDRINCL (headers are included in packet)
    if(setsockopt(sock, IPPROTO_IP, IP_HDRINCL, (char *)&one, sizeof(one)) < 0) {
        perror("Error while setting socket options");
        exit(-1);
    }

    return sock;
}

int send_raw_tcp_packet(int sock, 
                        struct sockaddr_in* src, 
						struct sockaddr_in* dst,  
						unsigned int seq, 
						unsigned int ack_seq,
                        unsigned short rst,
                        char* payload,
                        unsigned int payload_len
 ) {
    int bytes  = 1;
    struct iphdr *ipHdr;
    struct tcphdr *tcpHdr;

    //Initial guess for the SEQ field of the TCP header
//    unsigned int initSeqGuess = rand() * UINT32_MAX;

    //Data to be appended at the end of the tcp header
    char* data;

    //Ethernet header + IP header + TCP header + data
    char packet[1514];

    //Pseudo TCP header to calculate the TCP header's checksum
    struct pseudoTCPPacket pTCPPacket;

    //Pseudo TCP Header + TCP Header + data
    char *pseudo_packet;

    //Allocate mem for ip and tcp headers and zero the allocation
    memset(packet, 0, sizeof(packet));
    ipHdr = (struct iphdr *) packet;
    tcpHdr = (struct tcphdr *) (packet + sizeof(struct iphdr));
    data = (char *) (packet + sizeof(struct iphdr) + sizeof(struct tcphdr));
    if(payload && payload_len) 
        memcpy(data, payload, payload_len);

    //Populate ipHdr
    ipHdr->ihl = 5; //5 x 32-bit words in the header
    ipHdr->version = 4; // ipv4
    ipHdr->tos = 0;// //tos = [0:5] DSCP + [5:7] Not used, low delay
    ipHdr->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr) + strlen(data); //total lenght of packet. len(data) = 0
    ipHdr->id = htons(seed++); // 0x00; //16 bit id
    ipHdr->frag_off = 0x40; //16 bit field = [0:2] flags + [3:15] offset = 0x0
    ipHdr->ttl = 64; //16 bit time to live (or maximal number of hops)
    ipHdr->protocol = IPPROTO_TCP; //TCP protocol
    ipHdr->check = 0; //16 bit checksum of IP header. Can't calculate at this point
    ipHdr->saddr = src->sin_addr.s_addr; //32 bit format of source address
    ipHdr->daddr = dst->sin_addr.s_addr; //32 bit format of source address
//    memcpy(&ip->saddr, &srcaddr4->sin_addr, sizeof(unsigned int));
//    memcpy(&ip->daddr, &destaddr4->sin_addr, sizeof(unsigned int));

    //Now we can calculate the check sum for the IP header check field
    ipHdr->check = csum((unsigned short *) packet, ipHdr->tot_len);

    //Populate tcpHdr
    tcpHdr->source = src->sin_port; //16 bit in nbp format of source port
    tcpHdr->dest = dst->sin_port; //16 bit in nbp format of destination port
    tcpHdr->seq = seq;
    tcpHdr->ack_seq = ack_seq;
    tcpHdr->doff = 5; //4 bits: 5 x 32-bit words on tcp header
    tcpHdr->res1 = 0; //4 bits: Not used
    tcpHdr->cwr = 0; //Congestion control mechanism
    tcpHdr->ece = 0; //Congestion control mechanism
    tcpHdr->urg = 0; //Urgent flag
    tcpHdr->psh = 0; //Push data immediately
    if(rst){
        tcpHdr->ack = 1; //Acknownledge
        tcpHdr->rst = 1; //RST flag
    }
    else{
        tcpHdr->ack = 1; //Acknownledge
        tcpHdr->rst = 0; //RST flag
    }
    tcpHdr->syn = 0; //SYN flag
    tcpHdr->fin = 0; //Terminates the connection
    tcpHdr->window = htons(9638);//0xFFFF; //16 bit max number of databytes
    tcpHdr->check = 0; //16 bit check sum. Can't calculate at this point
    tcpHdr->urg_ptr = 0; //16 bit indicate the urgent data. Only if URG flag is set

    //Now we can calculate the checksum for the TCP header
    pTCPPacket.srcAddr = ipHdr->saddr; //32 bit format of source address
    pTCPPacket.dstAddr = ipHdr->daddr; //32 bit format of source address
    pTCPPacket.zero = 0; //8 bit always zero
    pTCPPacket.protocol = IPPROTO_TCP; //8 bit TCP protocol
    pTCPPacket.TCP_len = htons(sizeof(struct tcphdr) + strlen(data)); // 16 bit length of TCP header

    //Populate the pseudo packet
    pseudo_packet = (char *) malloc((int) (sizeof(struct pseudoTCPPacket) + sizeof(struct tcphdr) + strlen(data)));
    memset(pseudo_packet, 0, sizeof(struct pseudoTCPPacket) + sizeof(struct tcphdr) + strlen(data));

    //Copy pseudo header
    memcpy(pseudo_packet, (char *) &pTCPPacket, sizeof(struct pseudoTCPPacket));

    //Calculate check sum: zero current check, copy TCP header + data to pseudo TCP packet, update check
    tcpHdr->check = 0;

    //Copy tcp header + data to fake TCP header for checksum
    memcpy(pseudo_packet + sizeof(struct pseudoTCPPacket), tcpHdr, sizeof(struct tcphdr) + strlen(data));

    //Set the TCP header's check field
    tcpHdr->check = (csum((unsigned short *) pseudo_packet, (int) (sizeof(struct pseudoTCPPacket) +
                                                                    sizeof(struct tcphdr) +  strlen(data))));
    free(pseudo_packet);

    //Finally, send packet
    if((bytes = sendto(sock, packet, ipHdr->tot_len, 0, (struct sockaddr *)dst, sizeof(struct sockaddr))) < 0) {
        perror("Error on sendto()");
        return -1;
    }
    else {
        fprintf(stderr,"Success! Sent %d bytes.\n", bytes);
    }
    return 0;
}

void * interceptACK(void *pVoid) {
    uint8_t recvbuf[3000];
    struct sockaddr recvaddr;
    socklen_t len0 = sizeof(struct sockaddr);
    while (1) {
        recvfrom(raw_sock_rx, recvbuf, 3000, 0, &recvaddr, &len0);
        if (((struct iphdr*)recvbuf)->saddr == (uint32_t)pVoid) {
            struct tcphdr* tcpHeader = ((struct iphdr*)recvbuf) + 1;
            if (tcpHeader->syn == 1) {
                struct tcphdr *ptr = (struct tcphdr *) (recvbuf + sizeof(struct iphdr));
                seq = ntohl(ptr->ack_seq);
                ack_seq = (ntohl(ptr->seq) + 1);
                printf("Found seq and ack\n");
                break;
            }
        }
    }
    pthread_exit(NULL);
}

void* log_reply(void *pVoid){
    struct timeval tv = {1,0};
    uint8_t recvbuf[3000];
    struct sockaddr recvaddr;
    socklen_t len0 = sizeof(struct sockaddr);

    setsockopt(raw_sock_rx, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&tv, sizeof(struct timeval));
    while (1) {

        recvfrom(raw_sock_rx, recvbuf, 3000, 0, &recvaddr, &len0);
        if (((struct iphdr*)recvbuf)->saddr == (uint32_t)pVoid) {
            struct tcphdr* tcpHeader = ((struct iphdr*)recvbuf) + 1;
            if (tcpHeader->syn == 1) {
                struct tcphdr *ptr = (struct tcphdr *) (recvbuf + sizeof(struct iphdr));
                seq = ntohl(ptr->ack_seq);
                ack_seq = (ntohl(ptr->seq) + 1);
                printf("Found seq and ack\n");
                break;
            }
        }
    }    
}

uint32_t get_localip(char* intf){
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, intf, IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    return ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
}

void intHandler(int dummy){
    printf("Catch Ctrl+C\n");
    send_raw_tcp_packet(raw_sock_tx,&client,&server,htonl(seq),0,1,NULL,0);
    sleep(3);
    exit(0);
}

int main(int argc , char *argv[])
{
    int sock, bytes = 0, i, client_socklen;
    // struct timeval pkt_this_tv, pkt_last_tv, pkt_intvl_tv;
    // struct timeval ses_this_tv, ses_last_tv, ses_intvl_tv;
    char client_message[2000],*msg;
    char req_str[1448] = "GET /sdk-tools-linux-3859397.zip HTTP/1.1\r\nHost: 169.235.31.181\r\nConnection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\nReferer: http://169.235.31.181/\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: zh-CN,zh;q=0.9,en;q=0.8\r\n";
    char trick_str[] = "GET     ";
    char space_str[1448];
    for(i=0;i<1448;i++)
        space_str[i] = " ";
    pthread_t t1;

    if(argc < 3){
        perror("At least 2 arguments.");
        return -1;
    }
    uint32_t dstip_u32 = inet_addr(argv[1]);
    int dport = atoi(argv[2]);
    int sport = atoi(argv[3]);
    // char* intf = argv[4];

    srand(time(0));
    seed = rand() % 65536;
    raw_sock_tx = initRawSocket(IPPROTO_RAW);
    raw_sock_rx = initRawSocket(IPPROTO_TCP);
    pthread_create(&t1,0,interceptACK,dstip_u32);

    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        perror("Could not create socket\n");
        return -1;
    }
    printf("Socket created\n");

    server.sin_addr.s_addr = dstip_u32;
    server.sin_family = AF_INET;
    server.sin_port = htons(dport);

    client.sin_addr.s_addr = INADDR_ANY;
    client.sin_family = AF_INET;
    client.sin_port = htons(sport);

    int reuse = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

#ifdef SO_REUSEPORT
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
        perror("setsockopt(SO_REUSEPORT) failed");
#endif

    if (bind(sock, (struct sockaddr*) &client, sizeof(struct sockaddr_in)) < 0){
        printf("Unable to bind\n"); 
        return -1;
    } 

    //Connect to remote server
    while (connect(sock, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) < 0)
    {
        perror("connect failed. Retry");
    
    }     
    printf("Connected\n");     

    getsockname(sock,(struct sockaddr *)&client,&client_socklen);

    signal(SIGINT, intHandler);
    i = 0;
    while(1){
        if (!i)
            msg = trick_str;
        else
            msg = space_str;
        msg_len = strlen(msg);
        if((seq || ack_seq) && send_raw_tcp_packet(raw_sock_tx,&client,&server,htonl(seq),htonl(ack_seq),0,msg,msg_len) < 0) {
            printf("%d %d %d\n",seq,ack_seq,msg_len);
            printf("%s",msg);
            perror("Error on sendto()");
            return -1;
        }
        // int ram,i;
        // for(i = 0;i < 6;i++){
        //     ram = rand() % msg_len;
        //     msg[ram] = rand() * 255;
        // }
        seq += msg_len;
        sleep(1);
        i++;
        // printf("Success! Sent %d bytes.\n", bytes);
    }

    return 0;
}