#include <time.h>
#include <sys/time.h>
#include <linux/tcp.h>
#include <linux/ip.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>

unsigned int seed;

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
    unsigned int srcAddr;
    unsigned int dstAddr;
    unsigned char zero;
    unsigned char protocol;
    unsigned short TCP_len;
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

int send_raw_synack(int sock, 
                    unsigned int saddr,
                    unsigned int daddr,
                    unsigned short dport
 ) {
    int bytes  = 1;
    struct iphdr *ipHdr;
    struct tcphdr *tcpHdr;
    struct sockaddr_in dst;

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
    ipHdr->saddr = saddr; //32 bit format of source address
    ipHdr->daddr = daddr; //32 bit format of source address
//    memcpy(&ip->saddr, &srcaddr4->sin_addr, sizeof(unsigned int));
//    memcpy(&ip->daddr, &destaddr4->sin_addr, sizeof(unsigned int));

    //Now we can calculate the check sum for the IP header check field
    ipHdr->check = csum((unsigned short *) packet, ipHdr->tot_len);

    //Populate tcpHdr
    tcpHdr->source = htons(33456);
    tcpHdr->dest = dport; //16 bit in nbp format of destination port
    tcpHdr->seq = htonl(seed);
    tcpHdr->ack_seq = 0;
    tcpHdr->doff = 5; //4 bits: 5 x 32-bit words on tcp header
    tcpHdr->res1 = 0; //4 bits: Not used
    tcpHdr->cwr = 0; //Congestion control mechanism
    tcpHdr->ece = 0; //Congestion control mechanism
    tcpHdr->urg = 0; //Urgent flag
    tcpHdr->ack = 0; //Acknownledge
    tcpHdr->psh = 0; //Push data immediately
    tcpHdr->rst = 0; //RST flag
    tcpHdr->syn = 1; //SYN flag
    tcpHdr->fin = 0; //Terminates the connection
    tcpHdr->window = htons(9638);//0xFFFF; //16 bit max number of databytes
    tcpHdr->check = 0; //16 bit check sum. Can't calculate at this point
    tcpHdr->urg_ptr = 0; //16 bit indicate the urgent data. Only if URG flag is set

    //Now we can calculate the checksum for the TCP header
    pTCPPacket.srcAddr = ipHdr->saddr; //32 bit format of source address
    pTCPPacket.dstAddr = ipHdr->daddr; //32 bit format of source address
    pTCPPacket.zero = 0; //8 bit always zero
    pTCPPacket.protocol = IPPROTO_TCP; //8 bit TCP protocol
    pTCPPacket.TCP_len = htons(sizeof(struct tcphdr)); // 16 bit length of TCP header

    //Populate the pseudo packet
    pseudo_packet = (char *) malloc((int) (sizeof(struct pseudoTCPPacket) + sizeof(struct tcphdr)));
    memset(pseudo_packet, 0, sizeof(struct pseudoTCPPacket) + sizeof(struct tcphdr));

    //Copy pseudo header
    memcpy(pseudo_packet, (char *) &pTCPPacket, sizeof(struct pseudoTCPPacket));

    //Calculate check sum: zero current check, copy TCP header + data to pseudo TCP packet, update check
    tcpHdr->check = 0;

    //Copy tcp header + data to fake TCP header for checksum
    memcpy(pseudo_packet + sizeof(struct pseudoTCPPacket), tcpHdr, sizeof(struct tcphdr));

    //Set the TCP header's check field
    tcpHdr->check = (csum((unsigned short *) pseudo_packet, (int) (sizeof(struct pseudoTCPPacket) +
                                                                    sizeof(struct tcphdr))));
    dst.sin_family = AF_INET;
    dst.sin_addr.s_addr = ipHdr->daddr;
    dst.sin_port = tcpHdr->dest;

    //Finally, send packet
    if((bytes = sendto(sock, packet, ipHdr->tot_len, 0, (struct sockaddr *)&dst, sizeof(struct sockaddr))) < 0) {
        perror("Error on sendto()");
        return -1;
    }
    else {
        //fprintf(stderr,"Success! Sent %d bytes.\n", bytes);
    }
    return 0;
}


int reach_interval(struct timeval* thistime,struct timeval* lasttime,struct timeval* intervaltime){
    return thistime->tv_sec > lasttime->tv_sec + intervaltime->tv_sec
            || (thistime->tv_sec == lasttime->tv_sec + intervaltime->tv_sec
                && thistime->tv_usec >= lasttime->tv_usec + intervaltime->tv_usec);
}

void set_timeval(struct timeval* tv,const float ftime){
    int mpler = 1000000;
    int dt = ftime * mpler;
    tv->tv_sec =  (int)ftime;
    tv->tv_usec = dt%mpler; 
}

void print_utc_time(time_t sec){
    if (sec == -1) {
        puts("The time() function failed");
    }
        
    struct tm *ptm = gmtime(&sec);
    if (ptm == NULL) {
        puts("The gmtime() function failed");
    }    
    printf("UTC time: %s", asctime(ptm));
}


int main(int argc , char *argv[])
{    
    if(argc < 3){
        perror("At least 2 arguments.");
        return -1;
    }
    char* intf = argv[1];
    char* ip = argv[2];
    int port = strtol(argv[3], NULL, 10);

    int raw_sock_tx;
    raw_sock_tx = initRawSocket(IPPROTO_RAW);

    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, intf, IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    /* display result */
    unsigned int saddr = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
    unsigned int daddr = inet_addr(ip);
    unsigned short port_n = htons(port);

    struct timeval pkt_this_tv, pkt_last_tv, pkt_intvl_tv;
    set_timeval(&pkt_intvl_tv,0.2);
    gettimeofday(&pkt_last_tv, NULL);
    printf("The current local time is: %ld.%06ld\n",pkt_intvl_tv.tv_sec,pkt_intvl_tv.tv_usec);
    printf("The current local time is: %ld.%06ld\n",pkt_last_tv.tv_sec,pkt_last_tv.tv_usec);
    while(1){
        gettimeofday(&pkt_this_tv, NULL);
        // printf("The current local time is: %ld.%06ld\n",pkt_this_tv.tv_sec,pkt_this_tv.tv_usec);
        if(reach_interval(&pkt_this_tv,&pkt_last_tv,&pkt_intvl_tv)){
            printf("reach interval\n");
            pkt_last_tv = pkt_this_tv;
            send_raw_synack(raw_sock_tx,saddr,daddr,port_n);
            printf("send synack");
        }
        // printf("not reach\n");
    }
}