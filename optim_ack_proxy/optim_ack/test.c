
#include "hping2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>

#include <sys/stat.h>

#include <linux/netfilter.h>
#include <libnetfilter_queue/libnetfilter_queue.h>

#include "logging.h"
#include "util.h"
#include "cache.h"
#include "redis.h"
#include "socket.h"


#define NF_QUEUE_NUM 6




/* uncomment below unless you want to specify local ip */
//#define LOCAL_IP ""

/*
 * Options
 */

int opt_measure = 0;

/*
 * Global variables
 */

#define SUBCONN_NUM 3
// Optimistic Ack
struct subconn_info
{
    unsigned short local_port;
    unsigned int ini_seq_rem;//remote sequence number
};
struct subconn_info subconn_info[SUBCONN_NUM];
pthread_mutex_t mutex_subconn;
int ack_pacing;
unsigned int seq_next_global = 1;
int client_sock;
const int MARK = 66;
char* iptable_rules[100];
int iptable_rules_len = 0;



struct nfq_handle *g_nfq_h;
struct nfq_q_handle *g_nfq_qh;
int g_nfq_fd;

int nfq_stop;

pid_t tcpdump_pid = 0;

timespec start, end;

int type1rst, type2rst, succrst, succsynack;

char type1gfw[30], type2gfw[30];

unsigned char last_ttl;
unsigned char legal_ttl;

char pkt_data[10000];
size_t pkt_len;

char local_ip[16];
unsigned short local_port = 38324;

char remote_ip[16];
unsigned short remote_port = 80;

char local_host_name[64];
char remote_host_name[64];

#define BUF_SIZE 4096
char payload_sk[BUF_SIZE];// = "GET /?keyword=ultrasurf HTTP/1.1\r\nHOST: whatever.com\r\nUser-Agent: test agent\r\n\r\n";


static int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, 
              struct nfq_data *nfa, void *data);


int start_redis_server()
{
    int ret;
    log_info("Starting redis server.");
    ret = system("redis-server redis.conf");
    if (ret != 0) {
        log_error("Failed to start redis server.");
        return -1;
    }

    return 0;
}

int stop_redis_server()
{
    FILE *fp = fopen("redis.pid", "r");
    if (fp == NULL) {
        log_warn("Redis server is not running?");
        return -1;
    }

    char s[10] = "";
    fread(s, 1, 10, fp);
    pid_t redis_pid = strtol(s, NULL, 10);
    log_info("Killing redis server (pid %d).", redis_pid);
    kill(redis_pid, SIGTERM);

    return 0;
}

int setup_nfq()
{
    g_nfq_h = nfq_open();
    if (!g_nfq_h) {
        log_error("error during nfq_open()");
        return -1;
    }

    log_debug("unbinding existing nf_queue handler for AF_INET (if any)");
    if (nfq_unbind_pf(g_nfq_h, AF_INET) < 0) {
        log_error("error during nfq_unbind_pf()");
        return -1;
    }

    log_debug("binding nfnetlink_queue as nf_queue handler for AF_INET");
    if (nfq_bind_pf(g_nfq_h, AF_INET) < 0) {
        log_error("error during nfq_bind_pf()");
        return -1;
    }

    // set up a queue
    log_debug("binding this socket to queue %d", NF_QUEUE_NUM);
    g_nfq_qh = nfq_create_queue(g_nfq_h, NF_QUEUE_NUM, &cb, NULL);
    if (!g_nfq_qh) {
        log_error("error during nfq_create_queue()");
        return -1;
    }
    log_debug("nfq queue handler: %p", g_nfq_qh);

    log_debug("setting copy_packet mode");
    if (nfq_set_mode(g_nfq_qh, NFQNL_COPY_PACKET, 0xffff) < 0) {
        log_error("can't set packet_copy mode");
        return -1;
    }

    g_nfq_fd = nfq_fd(g_nfq_h);

    return 0;
}

int teardown_nfq()
{
    log_debug("unbinding from queue %d", NF_QUEUE_NUM);
    if (nfq_destroy_queue(g_nfq_qh) != 0) {
        log_error("error during nfq_destroy_queue()");
        return -1;
    }

#ifdef INSANE
    /* normally, applications SHOULD NOT issue this command, since
     * it detaches other programs/sockets from AF_INET, too ! */
    log_debug("unbinding from AF_INET");
    nfq_unbind_pf(g_nfq_h, AF_INET);
#endif

    log_debug("closing library handle");
    if (nfq_close(g_nfq_h) != 0) {
        log_error("error during nfq_close()");
        return -1;
    }

    return 0;
}

//
void generate_iptables_rules(char** rules_pool, int* pool_len, int local_port){
    char* cmd = (char*) malloc(200);
    sprintf(cmd, "INPUT -p tcp -s %s --sport %d --dport %d -m mark --mark %d -j ACCEPT", remote_ip, remote_port, local_port, MARK);
    rules_pool[(*pool_len)++] = cmd;
    printf("%d\n", *pool_len);

    cmd = (char*) malloc(200);
    sprintf(cmd, "INPUT -p tcp -s %s --sport %d --dport %d -j NFQUEUE --queue-num %d", remote_ip, remote_port, local_port, NF_QUEUE_NUM);
    rules_pool[(*pool_len)++] = cmd;
    printf("%d\n", *pool_len);



    cmd = (char*) malloc(200);
    sprintf(cmd, "OUTPUT -t raw -p tcp -d %s --dport %u --sport %u --tcp-flags RST,ACK RST -j DROP", remote_ip, remote_port, local_port);
    rules_pool[(*pool_len)++] = cmd;
    printf("%d\n", *pool_len);

    // cmd = (char*) malloc(200);
    // sprintf(cmd, "OUTPUT -t raw -p tcp -d %s --dport %d --sport %d  -j NFQUEUE --queue-num %d", remote_ip, remote_port, local_port, NF_QUEUE_NUM);
    // rules_pool[(*pool_len)++] = cmd;

    // cmd = (char*) malloc(200);
    // sprintf(cmd, "OUTPUT -t raw -p tcp -d %s --dport %d --sport %d  -m mark --mark %d -j ACCEPT", remote_ip, remote_port, local_port, MARK);
    // rules_pool[(*pool_len)++] = cmd;
    
}

void exec_iptables_rules(char** rules_pool, int start, int end, char action)
{
    char cmd[1000];
    for (int i = start; i < end; i++){
        printf("cmd %d: -%c %s\n", i, action, rules_pool[i];)
        sprintf(cmd, "iptables -%c %s", action, rules_pool[i]);
        system(cmd);
    }
}


// void exec_remove_iptables_rules()
// {
//     char cmd[1000];
//     sprintf(cmd, "iptables -D INPUT -p tcp -s %s --sport %d -j NFQUEUE --queue-num %d", remote_ip, remote_port, NF_QUEUE_NUM);
//     system(cmd);
//     sprintf(cmd, "iptables -D OUTPUT -t raw -p tcp -d %s --dport %d -j NFQUEUE --queue-num %d", remote_ip, remote_port, NF_QUEUE_NUM);
//     system(cmd);
// }

void cleanup()
{
    fin_log();

    teardown_nfq();

    // stop_redis_server();

    pthread_mutex_destroy(&mutex_subconn);

    exec_iptables_rules(iptable_rules, 0, iptable_rules_len, 'D');
    for(int i = 0; i < iptable_rules_len; i++)
        free(iptable_rules[i]);

}

void signal_handler(int signum)
{
    log_debug("Signal %d recved.", signum);
    cleanup();
    exit(EXIT_FAILURE);
}


void init()
{
    // init random seed
    srand(time(NULL));

    init_log();

    // initializing globals
    sockraw = open_sockraw();
    if (setsockopt(sockraw, SOL_SOCKET, SO_MARK, &MARK, sizeof(MARK)) < 0)
    {
        log_error("couldn't set mark\n");
        exit(1);
    }

    int portno = 80;
    sockpacket = open_sockpacket(portno);
    if (sockpacket == -1) {
        log_error("[main] can't open packet socket\n");
        exit(EXIT_FAILURE);
    }

    if (signal(SIGINT, signal_handler) == SIG_ERR) {
        log_error("register SIGINT handler failed.\n");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGSEGV, signal_handler) == SIG_ERR) {
        log_error("register SIGSEGV handler failed.");
        exit(EXIT_FAILURE);
    }

    if (setup_nfq() == -1) {
        log_error("unable to setup netfilter_queue");
        exit(EXIT_FAILURE);
    }

    // start_redis_server();

    // connect_to_redis();

    pthread_mutex_init(&mutex_subconn, NULL);
}



/* Process TCP packets
 * Return 0 to accept packet, otherwise to drop packet 
 */
int process_tcp_packet(struct mypacket *packet)
{
    struct myiphdr *iphdr = packet->iphdr;
    struct mytcphdr *tcphdr = packet->tcphdr;
    unsigned char *payload = packet->payload;

    char sip[16], dip[16];
    ip2str(iphdr->saddr, sip);
    ip2str(iphdr->daddr, dip);

    unsigned short sport, dport;
    unsigned int seq, ack;
    sport = ntohs(tcphdr->th_sport);
    dport = ntohs(tcphdr->th_dport);
    seq = tcphdr->th_seq;
    ack = tcphdr->th_ack;
    //log_debug("[TCP] This packet goes from %s:%d to %s:%d", sip, sport, dip, dport);
    //log_debug("TCP flags: %s", tcp_flags_str(tcphdr->th_flags));

    log_exp("%s:%d -> %s:%d <%s> seq %u ack %u ttl %u plen %d", sip, sport, dip, dport, tcp_flags_str(tcphdr->th_flags), ntohl(tcphdr->th_seq), ntohl(tcphdr->th_ack), iphdr->ttl, packet->payload_len);

    /* Check if the dest IP address is the one of our interface */
    if (inet_addr(local_ip) == iphdr->daddr)
    {
//      printf("destination IP does not match\n");
        return 0;
    }
    if (inet_addr(remote_ip) == iphdr->saddr)
    {
//      printf("source IP does not match\n");
        return 0;
    }

    // Find out which subconn
    int subconn_id = -1;
    for(int i = 0; i < SUBCONN_NUM; i++){
        if (subconn_info[i].local_port == dport){
            subconn_id = i;
            break;
        }
    }
    if (subconn_id == -1){
        log_error("process_tcp_packet: couldn't find subconn with port %d", dport);
    }
    printf("%d: found local port %d\n", subconn_id, dport);
    
    while(!subconn_info[subconn_id].ini_seq_rem); //make sure ini_seq_rem has been set
    printf("%d: seq-%d, ini_seq_rem-%d, seq_global %d\n", subconn_id, seq, subconn_info[subconn_id].ini_seq_rem, seq_next_global);
    if(seq - subconn_info[subconn_id].ini_seq_rem != seq_next_global)
        return 0;
    printf("%d: found segment\n", subconn_id);

    //find the exact segment, send it to the client
    if (send(client_sock, packet->payload, packet->payload_len, 0) <= 0){
        log_error("process_tcp_packet: send error %d", errno);
        return 0;
    }
    seq_next_global += packet->payload_len;
    printf("%d: sent segment to client, update seq_global\n", subconn_id);
    return 0;
}

static int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, 
              struct nfq_data *nfa, void *data)
{
    //log_debug("entering callback");
    //u_int32_t id = print_pkt(nfa);
    //char buf[1025];
    //nfq_snprintf_xml(buf, 1024, nfa, NFQ_XML_ALL);
    //log_debug("%s", buf);
    
    struct nfqnl_msg_packet_hdr *ph;
    ph = nfq_get_msg_packet_hdr(nfa);
    if (!ph) {
        log_error("nfq_get_msg_packet_hdr failed");
        return -1;
    }
    u_int32_t id = ntohl(ph->packet_id);
    //log_debug("packet id: %d", id);

    // get data (IP header + TCP header + payload)
    unsigned char *pkt_data;
    int plen = nfq_get_payload(nfa, &pkt_data);
    //if (plen >= 0)
    //    log_debug("payload_len=%d", plen);
    //hex_dump(pkt_data, plen);

    struct mypacket packet;
    packet.data = pkt_data;
    packet.len = plen;
    packet.iphdr = ip_hdr(pkt_data);
    
    // parse ip
    // char sip[16], dip[16];
    // ip2str(packet.iphdr->saddr, sip);
    // ip2str(packet.iphdr->daddr, dip);
    //log_debugv("This packet goes from %s to %s.", sip, dip);

    int ret = 0;

    switch (packet.iphdr->protocol) {
        case 6: // TCP
            packet.tcphdr = tcp_hdr(pkt_data);
            packet.payload = tcp_payload(pkt_data);
            packet.payload_len = packet.len - packet.iphdr->ihl*4 - packet.tcphdr->th_off*4;
            // show_packet(&packet);
            ret = process_tcp_packet(&packet);
            break;
        default:
            log_error("Invalid protocol: %d", packet.iphdr->protocol);
    }
    
    if (ret == 0)
        nfq_set_verdict(qh, id, NF_ACCEPT, 0, NULL);
    else
        nfq_set_verdict(qh, id, NF_DROP, 0, NULL);
        
    // return <0 to stop processing
    return 0;
}

void *nfq_loop(void *arg)
{
    int rv;
    char buf[65536];

    while (!nfq_stop) {
        rv = recv(g_nfq_fd, buf, sizeof(buf), MSG_DONTWAIT);
        if (rv >= 0) {
            //log_debug("%d", rv);
            //hex_dump((unsigned char *)buf, rv);
            //log_debugv("pkt received");
            nfq_handle_packet(g_nfq_h, buf, rv);
        }
        else {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                log_debug("recv() ret %d errno: %d", rv, errno);
            }
            usleep(100); //10000
        }
    }
}


int create_server_sock(char *addr, int port)
{
    int addrlen, s, on = 1, x;
    static struct sockaddr_in client_addr;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
        log_error("socket");

    addrlen = sizeof(client_addr);
    memset(&client_addr, '\0', addrlen);
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = inet_addr(addr);
    client_addr.sin_port = htons(port);
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, 4);
    x = bind(s, (struct sockaddr *)&client_addr, addrlen);
    if (x < 0)
        log_error("bind %s:%d", addr, port);

    x = listen(s, 5);
    if (x < 0)
        log_error("listen %s:%d", addr, port);
    log_exp("listening on %s port %d", addr, port);

    return s;
}

int wait_for_connection(int s)
{
    static int newsock;
    static socklen_t len;
    static struct sockaddr_in peer;

    len = sizeof(struct sockaddr);
    log_exp("calling accept FD %d", s);
    newsock = accept(s, (struct sockaddr *)&peer, &len);
    /* dump_sockaddr (peer, len); */
    if (newsock < 0)
    {
        if (errno != EINTR)
        {
            log_exp("accept FD %d: %s", s, strerror(errno));
            return -1;
        }
    }
    // get_hinfo_from_sockaddr(peer, len, client_hostname);
    // set_nonblock(newsock);
    printf("Accept one connection %d\n",newsock);
    return (newsock);
}


void* optimistic_ack(void* threadid)
{
    unsigned int seq, ack;
    int id = (long) threadid;
    void* voidptr = NULL;
    char pkt_data_local[10000];

    unsigned short local_port = subconn_info[id].local_port;

    seq = rand();

    int retry = 4;
    while (retry) {
        send_SYN("", 0, seq, local_port);
        printf("%d: Sent SYN\n", id);
        seq++;
        ack = wait_SYN_ACK(seq, 1, local_port, pkt_data_local);
        if (ack != 0) break;
        retry--;
    }
    if (retry == 0) {
        log_exp("Give up.");
        return voidptr;
    }
    printf("%d: Received SYN/ACK\n", id);
    pthread_mutex_lock(&mutex_subconn);
    subconn_info[id].ini_seq_rem = ack;
    pthread_mutex_unlock(&mutex_subconn);
    ack++;
    send_ACK(payload_sk, ack, seq, local_port);
    printf("%d: Sent ACK and request\n", id);
    seq += strlen(payload_sk);

    //Wait for first data packet
    int payload_len = wait_data(seq, ack, local_port, pkt_data_local);
    
    //Send Optim Acks, pacing
    printf("%d: Received first data, payload_len = %d, start optim ack\n", id, payload_len);
    for (int i = 1; !nfq_stop; i++){
        send_ACK("", ack+i*payload_len, seq, local_port);
        usleep(ack_pacing);
    }

}

int main(int argc, char *argv[])
{
    int opt;

    if (argc <= 4) {
        printf("Usage: %s <remote_ip> <remote_port> <local_port> <ack_pacing> \n", argv[0]);
        exit(-1);
    }

    strncpy(remote_ip, argv[1], 16);
    resolve((struct sockaddr*)&remote, remote_ip);

#ifndef LOCAL_IP
    get_local_ip(local_ip);
#else
    local_ip[0] = 0;
    strncat(local_ip, LOCAL_IP, 16);
#endif

    remote_port = atoi(argv[2]);
    local_port = atoi(argv[3]);

    // strncpy(remote_host_name, argv[4], 63);
    // strncpy(local_host_name, argv[5], 63);

    ack_pacing = atoi(argv[4]);

    /* records are saved in folder results */
    /* create the directory if not exist */
    mkdir("results", 0755);

    char hostname_pair_path[64], result_path[64];

    time_t rawtime;
    struct tm * timeinfo;
    char time_str[20];
    char tmp[64];

    sprintf(hostname_pair_path, "results/%s-%s", local_ip, remote_ip);
    mkdir(hostname_pair_path, 0755);

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(time_str, 20, "%Y%m%d_%H%M%S", timeinfo);
    sprintf(result_path, "%s/%s", hostname_pair_path, time_str);
    mkdir(result_path, 0755);

    init();

    // start the nfq proxy thread
    nfq_stop = 0;
    pthread_t nfq_thread;
    if (pthread_create(&nfq_thread, NULL, nfq_loop, NULL) != 0){
        log_error("Fail to create nfq thread.");
        exit(EXIT_FAILURE);
    }
    
    /* init experiment log */
    sprintf(tmp, "%s/experiment.log", result_path);
    init_exp_log(tmp);

    log_exp("Local IP: %s", local_ip);
    log_exp("Local Port: %d", local_port);
    log_exp("Remote IP: %s", remote_ip);
    log_exp("Remote Port: %d", remote_port);


    /* Current Workload
    ** ✅ 1. Listen on local server
    ** ✅ 2. Accept one connection, get the request payload
    ** ✅ 3. Add iptable rules, start intercept, callback send the first arrived packet to the local connection
    ** ✅ 4. Create 3 outgoing connections, send request, start optimistic ack
    ** ✅   4.1 Complete optimistic ack
    ** 5. Store the metadata of each subconn to recognize each in the intercept, local port, ack number, then there should be a global sequence number recording the current sequence number
    ** ✅ 6. Mutex for subconn_info
    ** ✅ 7. Mark packet sent by me
    */

    int master_sock;

    master_sock = create_server_sock(local_ip, local_port);
    for (;;)
    {
        if ((client_sock = wait_for_connection(master_sock)) < 0)
            continue;

        // Get the request payload
        int read_size;
        while ((read_size = recv(client_sock , payload_sk , BUF_SIZE , 0)) <= 0);
        printf("Receive client's request\n");
        pthread_t subconn_thread[SUBCONN_NUM];
        for (int i = 0; i < SUBCONN_NUM; i++){
            int local_port = rand() % 20000 + 30000; 
            subconn_info[i].local_port = local_port;//No nfq callback will interfere because iptable rules haven't been added
            subconn_info[i].ini_seq_rem = 0;
            printf("%d: local port = %d\n", i, local_port);

            // Add iptables rules
            generate_iptables_rules(iptable_rules, &iptable_rules_len, local_port);
            exec_iptables_rules(iptable_rules, iptable_rules_len-2, iptable_rules_len, 'A');
            printf("%d: iptables rules added\n", i);


            // Create outgoing connections
            if (pthread_create(&subconn_thread[i], NULL, optimistic_ack, (void *)i) != 0){
                log_error("Fail to create optimistic_ack thread.");
                exit(EXIT_FAILURE);
            }
            printf("%d: optimistic ack thread created\n", i);

        }

    }

    nfq_stop = 1;

    cleanup();

    return 0;
}
