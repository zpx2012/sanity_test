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

    struct iphdr *iph = (struct ipheader*)buf;          /* IPv4 header */
    struct tcphdr *tcph tcphdr = (struct tcpheader*)(buf + 20);        /* TCP header */
    u_int16_t sport, dport;           /* Source and destination ports */
    u_int32_t saddr, daddr;           /* Source and destination addresses */
    unsigned char *user_data;   /* TCP data begin pointer */
    unsigned char *it;          /* TCP data iterator */

    /* Convert network endianness to host endiannes */
    saddr = ntohl(iph->saddr);
    daddr = ntohl(iph->daddr);
    sport = ntohs(tcph->source);
    dport = ntohs(tcph->dest);

    /* Calculate pointers for begin and end of TCP packet data */
    user_data = (unsigned char *)((unsigned char *)tcph + (tcph->doff * 4));

    /* ----- Print all needed information from received TCP packet ------ */

    /* Print packet route */
    printf("print_tcp: %pI4h:%d -> %pI4h:%d\n", &saddr, sport,
                              &daddr, dport);

    /* Print TCP packet data (payload) */
    // printf("print_tcp: data:\n");
    // for (it = user_data; it != tail; ++it) {
    //     char c = *(char *)it;

    //     if (c == '\0')
    //         break;

    //     printf("%c", c);
    // }
    printf("\n\n");

 }

static int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, struct nfq_data *nfa, void *data)
{
        // u_int32_t id = print_pkt(nfa);
        u_int32_t id;
        char* data;

        struct nfqnl_msg_packet_hdr *ph;
        ph = nfq_get_msg_packet_hdr(nfa);    
        nfq_get_payload(nfa, &data);
        print_tcp_packet(data);   
        id = ntohl(ph->packet_id);
        printf("entering callback\n");
        return nfq_set_verdict(qh, id, NF_ACCEPT, 0, NULL);
}

struct nfq_handle *h;
struct nfq_q_handle *qh;
char dst_ip[16];

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

        int raw_sd;
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



        exit(0);
}


