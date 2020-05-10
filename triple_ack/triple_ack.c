#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <linux/types.h>
#include <linux/netfilter.h>            
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <signal.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "thr_pool.h"

struct nfq_handle *h;
struct nfq_q_handle *qh;
char DST_IP[16];
unsigned int SPORT;
int RAW_SD, MODE, COPY_NUM;
const int MARK = 3;

struct thread_data{
        int  id_rvs;
        int  len;
        unsigned char *buf;
};

static u_int32_t print_pkt (struct nfq_data *tb)
{
        int id = 0;
        struct nfqnl_msg_packet_hdr *ph;
        struct nfqnl_msg_packet_hw *hwph;
        u_int32_t mark,ifi; 
        int ret;
        unsigned char *data;

        ph = nfq_get_msg_packet_hdr(tb);
        if (ph) {
                id = ntohl(ph->packet_id);
                printf("hw_protocol=0x%04x hook=%u id=%u ",
                        ntohs(ph->hw_protocol), ph->hook, id);
        }

        hwph = nfq_get_packet_hw(tb);
        if (hwph) {
                int i, hlen = ntohs(hwph->hw_addrlen);

                printf("hw_src_addr=");
                for (i = 0; i < hlen-1; i++)
                        printf("%02x:", hwph->hw_addr[i]);
                printf("%02x ", hwph->hw_addr[hlen-1]);
        }

        mark = nfq_get_nfmark(tb);
        if (mark)
                printf("mark=%u ", mark);

        ifi = nfq_get_indev(tb);
        if (ifi)
                printf("indev=%u ", ifi);

        ifi = nfq_get_outdev(tb);
        if (ifi)
                printf("outdev=%u ", ifi);
        ifi = nfq_get_physindev(tb);
        if (ifi)
                printf("physindev=%u ", ifi);

        ifi = nfq_get_physoutdev(tb);
        if (ifi)
                printf("physoutdev=%u ", ifi);

        ret = nfq_get_payload(tb, &data);
        if (ret >= 0) {
                printf("payload_len=%d ", ret);
                //processPacketData (data, ret);
        }
        fputc('\n', stdout);

        return id;
}
        
void print_tcp_packet(unsigned char *buf) {

        struct iphdr *iph = (struct iphdr*)buf;          /* IPv4 header */
        struct tcphdr *tcph = (struct tcphdr*)(buf + 20);        /* TCP header */
        u_int16_t sport, dport;           /* Source and destination ports */
        struct sockaddr_in source, dest;

        /* Convert network endianness to host endiannes */
        memset(&source, 0, sizeof(source));
        source.sin_addr.s_addr = iph->saddr;
     
        memset(&dest, 0, sizeof(dest));
        dest.sin_addr.s_addr = iph->daddr;
        sport = ntohs(tcph->source);
        dport = ntohs(tcph->dest);

        /* ----- Print all needed information from received TCP packet ------ */

        /* Print packet route */
        printf("%s:%d -> ", inet_ntoa(source.sin_addr), sport);
        printf("%s:%d seq=%x ack=%x\n", inet_ntoa(dest.sin_addr), dport, ntohl(tcph->seq),ntohl(tcph->ack_seq));

}

int send_raw_packet(size_t sd, unsigned char *buf, uint16_t len) {
        //send packet on raw socket
        struct sockaddr_in sin; 
        sin.sin_family = AF_INET;
        sin.sin_port = ntohs(22);
        sin.sin_addr.s_addr = ((struct iphdr*)buf)->daddr;
        
        int ret = sendto(sd, buf, len, 0, (struct sockaddr*) &sin, sizeof(sin));
        if (ret < 0)
                fprintf(stderr, "send_raw_packet returns error\n");
        // else
        //         printf("send 1 packet.\n");
        return ret;
}




static int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, struct nfq_data *nfa, void *data)
{
        // u_int32_t id = print_pkt(nfa);
        unsigned char* packet;
        u_int32_t id_rvs = nfq_get_msg_packet_hdr(nfa)->packet_id;
        int packet_len = nfq_get_payload(nfa, &packet);

        struct thread_data* thr_data = malloc(sizeof(struct thread_data));
        if (!thr_data)
        {
                fprintf(stderr, "error during thr_data malloc\n");
                return -1;                                /* code */
        }
        thr_data->id_rvs = id_rvs;
        thr_data->len = packet_len;
        thr_data->buf = (unsigned char *)malloc(packet_len);
        if (!thr_data->buf){
                fprintf(stderr, "error during thr_data->buf malloc\n");
                return -1;
        }
        strncpy(thr_data->buf, packet, packet_len);
        if(thr_pool_queue(pool, pool_handler, (void *)thr_data) < 0){
                fprintf(stderr, "error during thr_pool_queue\n");
                return -1;
        }

        return 0;

        // printf("entering callback\n");
        // struct nfqnl_msg_packet_hdr *ph;
        // ph = nfq_get_msg_packet_hdr(nfa);    
        // id = ntohl(ph->packet_id);
        // packet_len = nfq_get_payload(nfa, &packet_data);
}

void pool_handler(struct thread_data* thr_data){
        u_int32_t id = ntohl(thr_data->id_rvs);
        unsigned int packet_len = thr_data->len;
        unsigned char* packet = thr_data->buf;
        struct iphdr  *iph = (struct iphdr*)packet_data;                 /* IPv4 header */
        struct tcphdr *tcph = (struct tcphdr*)(packet_data + 20);        /* TCP header */
        unsigned short payload_len = ntohs(iph->tot_len) - iph->ihl*4 - tcph->doff*4;

        // printf("payload_len:%d\n", payload_len);
        if ((!payload_len && !MODE) || (payload_len && MODE)){
                printf("sent %d packets len = %d.\n", COPY_NUM, payload_len);
                // print_tcp_packet(packet_data);   
                for (int i = 0; i < COPY_NUM; ++i)
                {
                        send_raw_packet(RAW_SD, packet_data, packet_len);
                }
        }
        if (nfq_set_verdict(qh, id, NF_ACCEPT, 0, NULL) < 0){
                fprintf(stderr, "error during nfq_set_verdict\n");
        }
        printf("free %p, %p\n", thr_data, thr_data->buf);
        free(thr_data->buf);
        free(thr_data);
}

void add_iprules(){
        char buf[200];
        if (MODE == 0){
                snprintf(buf,200,"iptables -A OUTPUT -d %s --protocol tcp --sport %d --tcp-flags ACK ACK -m mark --mark %d -j ACCEPT", DST_IP, SPORT, MARK); 
                system(buf);
                snprintf(buf,200,"iptables -A OUTPUT -d %s --protocol tcp --sport %d --tcp-flags ACK ACK -j NFQUEUE", DST_IP, SPORT); 
                system(buf);
        }
        else if (MODE == 1){
                snprintf(buf,200,"iptables -A OUTPUT -d %s --protocol tcp --dport %d --tcp-flags ACK ACK -m mark --mark %d -j ACCEPT", DST_IP, SPORT, MARK); 
                system(buf);
                snprintf(buf,200,"iptables -A OUTPUT -d %s --protocol tcp --dport %d --tcp-flags ACK ACK -j NFQUEUE", DST_IP, SPORT); 
                system(buf);               
        }
}

void delete_iprules(){
        char buf[200];
        if (MODE == 0){
                snprintf(buf,200,"iptables -D OUTPUT -d %s --protocol tcp --sport %d --tcp-flags ACK ACK -m mark --mark %d -j ACCEPT", DST_IP, SPORT, MARK); 
                system(buf);
                snprintf(buf,200,"iptables -D OUTPUT -d %s --protocol tcp --sport %d --tcp-flags ACK ACK -j NFQUEUE", DST_IP, SPORT); 
                system(buf);
        }
        else if (MODE == 1){
                snprintf(buf,200,"iptables -D OUTPUT -d %s --protocol tcp --dport %d --tcp-flags ACK ACK -m mark --mark %d -j ACCEPT", DST_IP, SPORT, MARK); 
                system(buf);
                snprintf(buf,200,"iptables -D OUTPUT -d %s --protocol tcp --dport %d --tcp-flags ACK ACK -j NFQUEUE", DST_IP, SPORT); 
                system(buf);               
        }
}

void close_nfq(){
        printf("unbinding from queue 0\n");
        nfq_destroy_queue(qh);

#ifdef INSANE
        /* normally, applications SHOULD NOT issue this command, since
         * it detaches other programs/sockets from AF_INET, too ! */
        printf("unbinding from AF_INET\n");
        nfq_unbind_pf(h, AF_INET);
#endif

        printf("closing library handle\n");
        nfq_close(h);
}



void INThandler(int sig)
{
        printf("entering INThandler\n");
        signal(sig, SIG_IGN);
        close_nfq();
        delete_iprules();
        exit(0);
}



int main(int argc, char **argv)
{
        signal(SIGINT, INThandler);

        int fd;
        int rv;
        char buf[4096] __attribute__ ((aligned));

        if (argc < 5){
                printf("Usage: dst_ip client_port COPY_NUM MODE[0:client/1:server]\n");
                exit(0);
        }
        strcpy(DST_IP, argv[1]);
        DST_IP[15] = '\0';
        SPORT = atoi(argv[2]);
        COPY_NUM = atoi(argv[3]);
        MODE = atoi(argv[4]);

        add_iprules();

        // thr_pool_t* pool = thr_pool_create(4, 10, 300, NULL);
        // if (!pool){
        //         fprintf(stderr, "couldn't create thr_pool\n");
        //         exit(1);                
        // }

        //setup of raw socket to send packets
        printf("setting up raw socket\n");
        RAW_SD = socket(PF_INET, SOCK_RAW, IPPROTO_RAW);
        if(RAW_SD < 0) {
                fprintf(stderr, "couldn't open RAW socket\n");
                exit(1);
        }

        if (setsockopt(RAW_SD, SOL_SOCKET, SO_MARK, &MARK, sizeof(MARK)) < 0)
        {
                fprintf(stderr, "couldn't set MARK\n");
                exit(1);
        }

        printf("opening library handle\n");
        h = nfq_open();
        if (!h) {
                fprintf(stderr, "error during nfq_open()\n");
                exit(1);
        }

        printf("unbinding existing nf_queue handler for AF_INET (if any)\n");
        if (nfq_unbind_pf(h, AF_INET) < 0) {
                fprintf(stderr, "error during nfq_unbind_pf()\n");
                exit(1);
        }

        printf("binding nfnetlink_queue as nf_queue handler for AF_INET\n");
        if (nfq_bind_pf(h, AF_INET) < 0) {
                fprintf(stderr, "error during nfq_bind_pf()\n");
                exit(1);
        }

        printf("binding this socket to queue '0'\n");
        qh = nfq_create_queue(h,  0, &cb, NULL);
        if (!qh) {
                fprintf(stderr, "error during nfq_create_queue()\n");
                exit(1);
        }

        printf("setting copy_packet MODE\n");
        if (nfq_set_MODE(qh, NFQNL_COPY_PACKET, 0xffff) < 0) {
                fprintf(stderr, "can't set packet_copy MODE\n");
                exit(1);
        }

        fd = nfq_fd(h);

        
        for (;;) {
                if ((rv = recv(fd, buf, sizeof(buf), 0)) >= 0) {
                        printf("pkt received\n");

                        printf("before nfq_handle_packet: buf %p\n", buf);
                        nfq_handle_packet(h, buf, rv);
                        // printf("after nfq_handle_packet\n");
                        continue;
                }
                /* if your application is too slow to digest the packets that
                * are sent from kernel-space, the socket buffer that we use
                * to enqueue packets may fill up returning ENOBUFS. Depending
                * on your application, this error may be ignored. Please, see
                * the doxygen documentation of this library on how to improve
                * this situation.
                */
                if (rv < 0 && errno == ENOBUFS) {
                        printf("losing packets!\n");
                        continue;
                }
                perror("recv failed");
                // break;
        }


        exit(0);
}


