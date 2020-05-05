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

struct nfq_handle *h;
struct nfq_q_handle *qh;
char dst_ip[16];
int raw_sd;


static u_int32_t print_pkt (struct nfq_data *tb)
{
        int id = 0;
        struct nfqnl_msg_packet_hdr *ph;
        struct nfqnl_msg_packet_hw *hwph;
        u_int32_t mark,ifi; 
        int ret;
        char *data;

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
        struct sockaddr_in source,dest;

        /* Convert network endianness to host endiannes */
        memset(&source, 0, sizeof(source));
        source.sin_addr.s_addr = iph->saddr;
     
        memset(&dest, 0, sizeof(dest));
        dest.sin_addr.s_addr = iph->daddr;
        sport = ntohs(tcph->source);
        dport = ntohs(tcph->dest);

        /* ----- Print all needed information from received TCP packet ------ */

        /* Print packet route */
        printf("print_tcp: %s:%d -> %s:%d seq=%x ack=%x\n", inet_ntoa(source.sin_addr), sport,
                              inet_ntoa(dest.sin_addr), dport, ntohl(tcph->seq),ntohl(tcph->ack_seq));

        printf("\n\n");

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
        else
                printf("send 1 packet.\n");
        return ret;
}




static int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, struct nfq_data *nfa, void *data)
{
        // u_int32_t id = print_pkt(nfa);
        u_int32_t id;
        unsigned char* packet_data;
        int packet_len, i;

        // printf("entering callback\n");
        struct nfqnl_msg_packet_hdr *ph;
        ph = nfq_get_msg_packet_hdr(nfa);    
        id = ntohl(ph->packet_id);

        packet_len = nfq_get_payload(nfa, &packet_data);
        print_tcp_packet(packet_data);   
        for (int i = 0; i < 3; ++i)
        {
                send_raw_packet(raw_sd, packet_data, packet_len);
        }
        return nfq_set_verdict(qh, id, NF_ACCEPT, 0, NULL);
}

void add_iprules(){
        char buf[100];
        snprintf(buf,100,"iptables -A OUTPUT -d %s --protocol tcp --tcp-flags ACK ACK -j NFQUEUE", dst_ip); 
        system(buf);
}

void delete_iprules(){
        char buf[100];
        snprintf(buf,100,"iptables -D OUTPUT -d %s --protocol tcp --tcp-flags ACK ACK -j NFQUEUE", dst_ip); 
        system(buf);
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



void  INThandler(int sig)
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

        if (argc < 2){
                printf("Please provide target IPaddress.\n");
                exit(0);
        }
        strcpy(dst_ip, argv[1]);
        dst_ip[15] = '\0';

        add_iprules();

        //setup of raw socket to send packets
        printf("setting up raw socket");
        raw_sd = socket(PF_INET, SOCK_RAW, IPPROTO_RAW);
        if(raw_sd < 0) {
                fprintf(stderr, "couldn't open RAW socket\n");
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

        printf("setting copy_packet mode\n");
        if (nfq_set_mode(qh, NFQNL_COPY_PACKET, 0xffff) < 0) {
                fprintf(stderr, "can't set packet_copy mode\n");
                exit(1);
        }

        fd = nfq_fd(h);

        // para el tema del loss:   while ((rv = recv(fd, buf, sizeof(buf), 0)) && rv >= 0)

        while ((rv = recv(fd, buf, sizeof(buf), 0)))
        {
                printf("pkt received\n");
                nfq_handle_packet(h, buf, rv);
        }
        
        for (;;) {
                if ((rv = recv(fd, buf, sizeof(buf), 0)) >= 0) {
                        printf("pkt received\n");
                        nfq_handle_packet(h, buf, rv);
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


