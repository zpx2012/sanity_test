
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

#include <iostream> 
#include <fstream>
#include <iterator>
#include <vector>
#include <set>

#include <linux/netfilter.h>
#include <libnetfilter_queue/libnetfilter_queue.h>
#include <linux/netlink.h>
#include <libnfnetlink/libnfnetlink.h>

#include "logging.h"
#include "util.h"
#include "socket.h"
#include "thr_pool.h"

/*
 * Bug 2020-06-20: Unresolved gaps
 *      - (1) Received the first fragment(256), lost the second, later other subconn sends full packet(536)
 *      _ (2) Received fragment(256), previous full packet is lost(536), two gaps(256) are inseted, later one full packet is received, one gap is deleted
 *
 * Bug 2020-06-20: In CN VPS, When ack_pacing < 5000, the connection goes stall
 *      Solved: queue or rcvbuffer is full, increased using nfq_set_queue_maxlen, nfnl_rcvbufsiz
*/



/* Todo
 * 1. Multiple request?
*/



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
#ifndef SUBCONN_NUM
#define SUBCONN_NUM 3
#endif
// Optimistic Ack
struct subconn_info
{
    unsigned short local_port;
    unsigned int ini_seq_rem;  //remote sequence number
    unsigned int ini_seq_loc;  //local sequence number
    unsigned int cur_seq_rem;
    unsigned int cur_seq_loc;
    short ack_sent;

    pthread_t thread;
    pthread_mutex_t mutex_opa;
    unsigned int optim_ack_stop;
    unsigned int opa_seq_start;  // local sequence number for optim ack to start
    unsigned int opa_ack_start;  // local ack number for optim ack to start
    unsigned int opa_seq_max_restart;
    unsigned int opa_retrx_counter;
    unsigned int win_size;
    int ack_pacing;
    unsigned int payload_len;
};
struct subconn_info subconn_infos[SUBCONN_NUM];
int ack_pacing;
unsigned int seq_next_global = 1;
int client_sock;
const int MARK = 66;
char* iptable_rules[100];
int iptable_rules_len = 0;
std::set<unsigned int> seq_gaps;
int optim_ack_stop = 1;
char hostname_pair_path[64], result_path[64];
int server_payload_len = 0;

//Multithread
struct thread_data{
    unsigned int  pkt_id;
    unsigned int  len;
    unsigned char *buf;
};
thr_pool_t* pool;
pthread_mutex_t mutex_seq_next_global = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_seq_gaps = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_subconn_infos = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_optim_ack_stop = PTHREAD_MUTEX_INITIALIZER;


//Original 
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
    if (nfq_set_mode(g_nfq_qh, NFQNL_COPY_PACKET, 0x0fff) < 0) {
        log_error("can't set packet_copy mode");
        return -1;
    }

#define NFQLENGTH 1024*200
#define BUFLENGTH 4096
    if (nfq_set_queue_maxlen(g_nfq_qh, NFQLENGTH) < 0) {
        log_error("error during nfq_set_queue_maxlen()\n");
        return -1;
    }
    struct nfnl_handle* nfnl_hl = nfq_nfnlh(g_nfq_h);
    nfnl_rcvbufsiz(nfnl_hl, NFQLENGTH * BUFLENGTH);

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

    cmd = (char*) malloc(200);
    // sprintf(cmd, "INPUT -m conntrack --ctstate NEW,ESTABLISHED -p tcp -s %s --sport %d --dport %d -j NFQUEUE --queue-num %d", remote_ip, remote_port, local_port, NF_QUEUE_NUM);
    sprintf(cmd, "INPUT -p tcp -s %s --sport %d --dport %d -j NFQUEUE --queue-num %d", remote_ip, remote_port, local_port, NF_QUEUE_NUM);
    // sprintf(cmd, "INPUT -p tcp -s %s --sport %d --dport %d -j NFQUEUE --queue-balance 0:%d", remote_ip, remote_port, local_port, NF_QUEUE_NUM);
    rules_pool[(*pool_len)++] = cmd;

    cmd = (char*) malloc(200);
    sprintf(cmd, "OUTPUT -t raw -p tcp -d %s --dport %u --sport %u --tcp-flags RST,ACK RST -j DROP", remote_ip, remote_port, local_port);
    rules_pool[(*pool_len)++] = cmd;

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
        log_exp("cmd %d: -%c %s", i, action, rules_pool[i]);
        sprintf(cmd, "iptables -%c %s", action, rules_pool[i]);
        system(cmd);
    }
}

void cleanup()
{
    fin_log();

    teardown_nfq();

    pthread_mutex_destroy(&mutex_seq_next_global);
    pthread_mutex_destroy(&mutex_seq_gaps);
    pthread_mutex_destroy(&mutex_subconn_infos);
    pthread_mutex_destroy(&mutex_optim_ack_stop);

    exec_iptables_rules(iptable_rules, 0, iptable_rules_len, 'D');
    for(int i = 0; i < iptable_rules_len; i++)
        free(iptable_rules[i]);

}

void signal_handler(int signum)
{
    log_debug("Signal %d recved.", signum);
    if(signum == SIGPIPE){
        log_exp("Receive EPIPE.");
        return;
    }
    nfq_stop = 1;
    cleanup();
    exit(EXIT_FAILURE);
}

void init_subconn(){
    for(int i = 0; i < SUBCONN_NUM;i++)
        memset(&subconn_infos[i], 0, sizeof(struct subconn_info));
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
    if (signal(SIGPIPE, signal_handler) == SIG_ERR) {
        log_error("register SIGPIPE handler failed.");
        exit(EXIT_FAILURE);
    }


    if (setup_nfq() == -1) {
        log_error("unable to setup netfilter_queue");
        exit(EXIT_FAILURE);
    }

    init_subconn();

    // pthread_mutex_init(&mutex_seq_next_global, NULL);
    // pthread_mutex_init(&mutex_seq_gaps, NULL);

    pool = thr_pool_create(4, 16, 300, NULL);
    if (!pool){
            log_error("couldn't create thr_pool\n");
            exit(1);                
    }
}


int find_seq_gaps(unsigned int seq){
    if (seq < *seq_gaps.begin())
        return 0;
    return seq_gaps.find(seq) != seq_gaps.end();
    // for (size_t i = 0; i < seq_gaps.size(); i++)
    // {
    //     if (seq < seq_gaps.at(i))
    //         return -1;
    //     else if(seq == seq_gaps.at(i))
    //         return i;
    // }
    // return -1;
}

void insert_seq_gaps(unsigned int start, unsigned int end, unsigned int step){
    for(; start < end; start += step){
        log_exp("insert gap %u", start);
        seq_gaps.insert(start);
    }

    // unsigned int last = seq_gaps.at(seq_gaps.size()-1);
    // if (start > last){
    //     for(; start < end; start += step)
    //         seq_gaps.push_back(start);
    // }
    // else if (start < last) {
    //     for(; start < end; start += step){

    //     }       
    // }
}

void delete_seq_gaps(unsigned int val){
    seq_gaps.erase(val);
}

void save_seq_gaps_to_file(){
    char tmp[64];
    sprintf(tmp, "%s/seq_gaps.txt", result_path);
    std::ofstream output_file(tmp);
    std::ostream_iterator<unsigned int> output_iterator(output_file, "\n");
    std::copy(seq_gaps.begin(), seq_gaps.end(), output_iterator);
}

int start_optim_ack(int id, unsigned int seq, unsigned int ack, unsigned int payload_len, unsigned int seq_max){
    subconn_infos[id].opa_seq_start = ack;
    subconn_infos[id].opa_ack_start = seq + 1;
    subconn_infos[id].opa_seq_max_restart = seq_max;
    subconn_infos[id].opa_retrx_counter = 0;
    subconn_infos[id].payload_len = payload_len;
    subconn_infos[id].optim_ack_stop = 0;
    pthread_t thread;
    if (pthread_create(&thread, NULL, optimistic_ack, (void *)(intptr_t)id) != 0){
        log_error("Fail to create optimistic_ack thread.");
        return -1;
    }
    log_exp("S%d: optimistic ack thread created", id);   
}

/* Bug: sub connections are not syncronized; one is way ahead, the others fall behind
 * Solution: 
 *      1. Wait until all connections finishe threeway handshake, then send request all together
 *      2. Wait until all connections receive the first data packet, then start optimistic ack, opt ack thread send one ack for each subconn each round
 *      3. Keep an array to track the gaps, while get global seq going and send discontinued packets to client, later send the gaps when arrived
*/

/* Process TCP packets
 * Return 0 to accept packet, otherwise to drop packet 
 */

int process_tcp_packet(struct thread_data* thr_data){

    // log_exp("process_tcp_packet: id %d", htonl(thr_data->pkt_id));
    struct myiphdr *iphdr = ip_hdr(thr_data->buf);
    struct mytcphdr *tcphdr = tcp_hdr(thr_data->buf);
    unsigned char *payload = tcp_payload(thr_data->buf);
    unsigned int payload_len = htons(iphdr->tot_len) - iphdr->ihl*4 - tcphdr->th_off*4;


    char sip[16], dip[16];
    ip2str(iphdr->saddr, sip);
    ip2str(iphdr->daddr, dip);

    unsigned short sport, dport;
    unsigned int seq, ack;
    sport = ntohs(tcphdr->th_sport);
    dport = ntohs(tcphdr->th_dport);
    seq = htonl(tcphdr->th_seq);
    ack = htonl(tcphdr->th_ack);

    /* Check if the dest IP address is the one of our interface */
    if (inet_addr(local_ip) != iphdr->daddr)
    {
        log_exp("destination IP does not match: %x %x",inet_addr(local_ip), iphdr->daddr);
        return 0;
    }
    if (inet_addr(remote_ip) != iphdr->saddr)
    {
        log_exp("source IP does not match %x %x",inet_addr(remote_ip), iphdr->saddr);
        return 0;
    }

    // Find out which subconn
    int subconn_id = -1;
    for(int i = 0; i < SUBCONN_NUM; i++){
        if (subconn_infos[i].local_port == dport){
            subconn_id = i;
            break;
        }
    }
    if (subconn_id == -1){
        log_error("process_tcp_packet: couldn't find subconn with port %d", dport);
        return -1;
    }
    log_exp("S%d-%d: %s:%d -> %s:%d <%s> seq %x(%u) ack %x(%u) ttl %u plen %d", subconn_id, thr_data->pkt_id, sip, sport, dip, dport, tcp_flags_str(tcphdr->th_flags), tcphdr->th_seq, seq-subconn_infos[subconn_id].ini_seq_rem, tcphdr->th_ack, ack-subconn_infos[subconn_id].ini_seq_loc, iphdr->ttl, payload_len);
    // log_exp("tot_len %d, thr_data->len %d", htons(iphdr->tot_len), thr_data->len);
    /*
    * 1. Received SYN/ACK: find the local port, send ACK and request
    * 2. Received data packet: send it to client
    *        if seq - init_seq == 1: //first data packet
    *            if subconn_infos[id].optimack_start == false:
    *                  get the payload len, start the optim ack
    *    
    */
    switch (tcphdr->th_flags){
        case TH_SYN|TH_ACK:
        {
            send_ACK("", seq+1, ack, dport);
            subconn_infos[subconn_id].ini_seq_rem = subconn_infos[subconn_id].cur_seq_rem = seq;
            subconn_infos[subconn_id].ack_sent = 1;            
            log_exp("%d: Received SYN/ACK. Sent ACK", subconn_id);
            
            //check if all subconns receive syn/ack
            int all_ack_sent = 1;
            pthread_mutex_lock(&mutex_subconn_infos);
            for (int i = 0; i < SUBCONN_NUM; i++)
                if (!subconn_infos[i].ack_sent){
                    all_ack_sent = 0;
                    break;
                }
            pthread_mutex_unlock(&mutex_subconn_infos);
            if (all_ack_sent){
                for (int i = 0; i < SUBCONN_NUM; i++){
                    send_ACK(payload_sk, subconn_infos[i].ini_seq_rem+1, subconn_infos[i].ini_seq_loc+1, subconn_infos[i].local_port);
                }
                log_exp("All ACK sent, sent request");
            }
            return -1;
            break;
        }
        
        case TH_ACK:
        case TH_ACK | TH_PUSH:
        case TH_ACK | TH_URG: 
        {
            if(!payload_len)
                return -1;

            if (!subconn_infos[subconn_id].ini_seq_rem){
                log_error("process_tcp_packet: ini_seq_rem not set");
                return -1;
            }
            unsigned int seq_rel = seq - subconn_infos[subconn_id].ini_seq_rem;
            
            if (seq_rel == 1 && subconn_infos[subconn_id].optim_ack_stop){
                start_optim_ack(subconn_id, seq, ack, payload_len, 0);

            }
            // else if(seq < subconn_infos[subconn_id].cur_seq_rem && seq >= subconn_infos[subconn_id].opa_seq_max_restart){
            //     // Retrnx
            //     // add mutex
            //     subconn_infos[subconn_id].opa_retrx_counter++;
            //     if (subconn_infos[subconn_id].opa_retrx_counter > 6){
            //         subconn_infos[subconn_id].optim_ack_stop = 1;
            //         subconn_infos[subconn_id].ack_pacing -= 10;
            //         while(subconn_infos[subconn_id].optim_ack_stop);
            //         log_exp("S%d: Restart optim ack", subconn_id);
            //         start_optim_ack(subconn_id, seq, ack, payload_len, subconn_infos[subconn_id].cur_seq_rem);
            //     }
            // }
            else {
                subconn_infos[subconn_id].cur_seq_rem = seq;
            }
            pthread_mutex_unlock(&subconn_infos[subconn_id].mutex_opa);


            // pthread_mutex_lock(&mutex_optim_ack_stop);
            // if (optim_ack_stop){
            //     optim_ack_stop = 0;
            //     pthread_t thread;
            //     if (pthread_create(&thread, NULL, optimistic_ack, (void *)(intptr_t)payload_len) != 0){
            //         log_error("Fail to create optimistic_ack thread.");
            //         pthread_mutex_unlock(&mutex_optim_ack_stop);
            //         exit(EXIT_FAILURE);
            //     }
            //     log_exp("\n-------------------\n\noptimistic ack thread created\n\n-------------------");
            // }
            // pthread_mutex_unlock(&mutex_optim_ack_stop);

            // if (seq_rel == 1 && !subconn_infos[subconn_id].payload_len){
            //     subconn_infos[subconn_id].payload_len = payload_len;
            //     log_exp("subconn_info updated");

            //     pthread_mutex_lock(&mutex_optim_ack_stop);
            //     if (optim_ack_stop) {
            //         // check if all subconns received 
            //         int all_data_recv = 1;
            //         for (int i = 0; i < SUBCONN_NUM; i++)
            //             if (!subconn_infos[i].payload_len){
            //                 all_data_recv = 0;
            //                 break;
            //             }
            //         if (all_data_recv){
            //             // Create outgoing connections
            //             optim_ack_stop = 0;
            //             pthread_t thread;
            //             if (pthread_create(&thread, NULL, optimistic_ack, NULL) != 0){
            //                 log_error("Fail to create optimistic_ack thread.");
            //                 exit(EXIT_FAILURE);
            //             }
            //             log_exp("\n-------------------\n\noptimistic ack thread created\n\n-------------------");
            //         }
            //     }
            //     pthread_mutex_unlock(&mutex_optim_ack_stop);
            // }
            // else if(seq_rel != 1 && !subconn_infos[subconn_id].payload_len){
            //     log_error("Not first data packet but subconn_info is not updated.\n");
            //     return -1;
            // }

            /* if seq > seq_global and seq-seq_global % payload_len == 0
            *      add all seq in between to the vector
            * else if seq < seq_global:
            *      search the vector
            *      if found, send the data, erase the seq from the vector
            *
            */

            pthread_mutex_lock(&mutex_seq_next_global);

            int offset = seq_rel - seq_next_global;
            unsigned int append = 0;
            if(offset > 0){
                // if ((seq_rel-seq_next_global) % packet->payload_len != 0){
                //     log_error("seq_rel %d-seq_next_global %d) % packet->payload_len %d != 0", seq_rel, seq_next_global, packet->payload_len);
                //     return -1;
                // }
                log_exp("S%d-%d: Insert gaps: %d, to: %d.", subconn_id, thr_data->pkt_id, seq_next_global, seq_rel);
                // pthread_mutex_lock(&mutex_seq_gaps);
                insert_seq_gaps(seq_next_global, seq_rel, payload_len);
                // pthread_mutex_unlock(&mutex_seq_gaps);
                append = offset + payload_len;

            }
            else if (offset < 0){
                
                int ret = find_seq_gaps(seq_rel);
                if (!ret){
                    log_exp("S%d-%d: recv %d < wanting %d", subconn_id, thr_data->pkt_id, seq_rel, seq_next_global);
                    pthread_mutex_unlock(&mutex_seq_next_global);
                    
                    pthread_mutex_lock(&subconn_infos[subconn_id].mutex_opa);
                    if(seq < subconn_infos[subconn_id].cur_seq_rem){//Retrx  && seq >= subconn_infos[subconn_id].opa_seq_max_restart
                        subconn_infos[subconn_id].opa_retrx_counter++;
                        if (subconn_infos[subconn_id].opa_retrx_counter > 6){
                            subconn_infos[subconn_id].optim_ack_stop = 1;
                            subconn_infos[subconn_id].ack_pacing -= 10;
                            subconn_infos[subconn_id].opa_retrx_counter = 0;
                            while(subconn_infos[subconn_id].optim_ack_stop);
                            start_optim_ack(subconn_id, seq, ack, payload_len, subconn_infos[subconn_id].cur_seq_rem);
                            log_exp("S%d-%d: Restart optim ack", subconn_id, thr_data->pkt_id);
                        }
                    }
                    pthread_mutex_unlock(&subconn_infos[subconn_id].mutex_opa);
                    return -1;
                }
                // pthread_mutex_lock(&mutex_seq_gaps);
                delete_seq_gaps(seq_rel);
                // pthread_mutex_unlock(&mutex_seq_gaps);
                log_exp("S%d-%d: Found gap %u. Delete gap.", subconn_id, thr_data->pkt_id, seq_rel);
            }
            else {
                append = payload_len;
                log_exp("S%d-%d: Found seg %u", subconn_id, thr_data->pkt_id, seq_rel);
            }

            if(append){
                seq_next_global += append;
                log_exp("S%d-%d: Update seq_global to %u", subconn_id, thr_data->pkt_id, seq_next_global);
            }

            pthread_mutex_unlock(&mutex_seq_next_global);

            //send it to the client
            if (send(client_sock, payload, payload_len, 0) <= 0){
                log_error("process_tcp_packet: send error %d", errno);
                return -1;
            }
            log_exp("S%d-%d: Sent seg %d to client\n", subconn_id, thr_data->pkt_id, seq_rel);

            return -1;
            break;
                
        }
        case TH_FIN|TH_ACK:
        {
            send_FIN_ACK("", seq+1, ack, dport);
            optim_ack_stop = 1;
            log_exp("Subconn %d: Received FIN/ACK. Sent FIN/ACK. Stop optim ack thread.", subconn_id);
            if (!seq_gaps.size()){
                log_exp("No gaps left. Close client connection.");
                close(client_sock);
            }
            else{
                log_exp("Gap left: %d", seq_gaps.size());
                save_seq_gaps_to_file();
                // for(std::set<unsigned int>::iterator itr=seq_gaps.begin(); itr != seq_gaps.end();itr++)
                //     printf("%u ", *itr);
                printf("\n");
            }
            return -1;
            break;
        }
        default:
            log_error("Invalid tcp flags: %s",tcp_flags_str(tcphdr->th_flags));
    }

    // log_exp("%d: seq-%d, ini_seq_rem-%d, seq_global %d\n", subconn_id, seq, subconn_infos[subconn_id].ini_seq_rem, seq_next_global);

    return 0;
}


void* pool_handler(void* arg){
    struct thread_data* thr_data = (struct thread_data*)arg;
    u_int32_t id = thr_data->pkt_id;
    int ret = -1;

    // log_exp("pool_handler: %d", id);
    short protocol = ip_hdr(thr_data->buf)->protocol;
    if (protocol == 6)
        ret = process_tcp_packet(thr_data);
    else{ 
        log_error("Invalid protocol: 0x%04x, len %d", protocol, thr_data->len);
        struct myiphdr *iphdr = ip_hdr(thr_data->buf);
        struct mytcphdr *tcphdr = tcp_hdr(thr_data->buf);
        unsigned char *payload = tcp_payload(thr_data->buf);
        unsigned int payload_len = thr_data->len - iphdr->ihl*4 - tcphdr->th_off*4;
        char sip[16], dip[16];
        ip2str(iphdr->saddr, sip);
        ip2str(iphdr->daddr, dip);

        log_exp("%s:%d -> %s:%d <%s> seq %x(%u) ack %x(%u) ttl %u plen %d", sip, ntohs(tcphdr->th_sport), dip, ntohs(tcphdr->th_dport), tcp_flags_str(tcphdr->th_flags), tcphdr->th_seq, tcphdr->th_ack, iphdr->ttl, payload_len);
        char* hex_str = hex_dump_str(thr_data->buf, thr_data->len);
        log_exp(hex_str);
        free(hex_str);
    }

    // free(thr_data->buf);
    free(thr_data);

    if (ret == 0){
        nfq_set_verdict(g_nfq_qh, id, NF_ACCEPT, 0, NULL);
        // log_exp("verdict: accpet\n");
    }
    else{
        nfq_set_verdict(g_nfq_qh, id, NF_DROP, 0, NULL);
        // log_exp("verdict: drop\n");
    }
}



static int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, struct nfq_data *nfa, void *data)
{
        unsigned char* packet;
        int packet_len = nfq_get_payload(nfa, &packet);

        struct thread_data* thr_data = (struct thread_data*)malloc(sizeof(struct thread_data));
        if (!thr_data)
        {
                log_error("cb: error during thr_data malloc\n");
                return -1;                                /* code */
        }
        memset(thr_data, 0, sizeof(struct thread_data));
        // log_exp("cb: id %d, protocol 0x%04x", ntohl(nfq_get_msg_packet_hdr(nfa)->packet_id), nfq_get_msg_packet_hdr(nfa)->hw_protocol);
        thr_data->pkt_id = htonl(nfq_get_msg_packet_hdr(nfa)->packet_id);
        thr_data->len = packet_len;
        thr_data->buf = (unsigned char *)malloc(packet_len);
        if (!thr_data->buf){
                log_error("cb: error during malloc\n");
                return -1;
        }
        memcpy(thr_data->buf, packet, packet_len);
        // char* hex_str = hex_dump_str(thr_data->buf, thr_data->len);
        // log_exp(hex_str);
        // free(hex_str);

        if(thr_pool_queue(pool, pool_handler, (void *)thr_data) < 0){
                log_error("cb: error during thr_pool_queue\n");
                return -1;
        }

        return 0;
}

// static int cb_old(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, 
//               struct nfq_data *nfa, void *data)
// {

//     struct nfqnl_msg_packet_hdr *ph;
//     ph = nfq_get_msg_packet_hdr(nfa);
//     if (!ph) {
//         log_error("nfq_get_msg_packet_hdr failed");
//         return -1;
//     }
//     u_int32_t id = ntohl(ph->packet_id);
//     //log_debug("packet id: %d", id);

//     // get data (IP header + TCP header + payload)
//     unsigned char *pkt_data;
//     int plen = nfq_get_payload(nfa, &pkt_data);
//     log_exp("id: %d, plen %d", id, plen);

//     struct mypacket packet;
//     packet.data = pkt_data;
//     packet.len = plen;
//     packet.iphdr = ip_hdr(pkt_data);

//     int ret = 0;

//     switch (packet.iphdr->protocol) {
//         case 6: // TCP
//             packet.tcphdr = tcp_hdr(pkt_data);
//             packet.payload = tcp_payload(pkt_data);
//             packet.payload_len = packet.len - packet.iphdr->ihl*4 - packet.tcphdr->th_off*4;
//             ret = process_tcp_packet(&packet);
//             break;
//         default:
//             log_error("Invalid protocol: %d", packet.iphdr->protocol);
//     }
    
//     // nfq_set_verdict(qh, id, NF_ACCEPT, 0, NULL);
//     // log_exp("verdict: accpet");
//     if (ret == 0){
//         nfq_set_verdict(qh, id, NF_ACCEPT, 0, NULL);
//         log_exp("verdict: accpet\n");
//     }
//     else{
//         nfq_set_verdict(qh, id, NF_DROP, 0, NULL);
//         log_exp("verdict: drop\n");
//     }
        
//     // return <0 to stop processing
//     return 0;
// }

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
            usleep(10); //10000
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
    log_exp("Accept one connection %d\n",newsock);
    return (newsock);
}



void* optimistic_ack(void* threadid){
    int id = (long) threadid;
    unsigned int ack_step = subconn_infos[id].payload_len;
    unsigned int opa_seq_start = subconn_infos[id].opa_seq_start;
    unsigned int opa_ack_start = subconn_infos[id].opa_ack_start;
    unsigned int local_port = subconn_infos[id].local_port;
    unsigned int ack_pacing = subconn_infos[id].ack_pacing;

    log_exp("S%d: Optim ack starts", id);
    for (int k = 0; !subconn_infos[id].optim_ack_stop; k++){
        send_ACK("",opa_ack_start+k*ack_step, opa_seq_start, local_port);
        usleep(ack_pacing);
    }
    subconn_infos[id].optim_ack_stop = 0;
    log_exp("S%d: Optim ack ends", id);
    pthread_exit(NULL);        
}

// void* optimistic_ack_all(void* threadid){
//     unsigned int payload_len = (long) threadid;

//     log_exp("Optim ack starts");
//     for (int k = 0; !optim_ack_stop ; k++){
//         for (int i = 0; i < SUBCONN_NUM; i++){
//             send_ACK("",subconn_infos[i].ini_seq_rem+1+k*payload_len, subconn_infos[i].cur_seq_loc, subconn_infos[i].local_port);
//             // log_exp("\nS%d: ack %u sent\n", i, 1+k*payload_len);
//         }
//         usleep(ack_pacing);
//     }
//     log_exp("Optim ack ends");
//     pthread_exit(NULL);
// }

// void* optimistic_ack_old(void* threadid)
// {
//     unsigned int seq, ack;
//     int id = (long) threadid;
//     void* voidptr = NULL;
//     char pkt_data_local[10000];

//     unsigned short local_port = subconn_infos[id].local_port;

//     seq = rand();

//     int retry = 4;
//     while (retry) {
//         send_SYN("", 0, seq, local_port);
//         printf("%d: Sent SYN\n", id);
//         seq++;
//         ack = wait_SYN_ACK(seq, 5, local_port, pkt_data_local);
//         if (ack != 0) break;
//         retry--;
//     }
//     if (retry == 0) {
//         log_exp("Give up.");
//         return voidptr;
//     }
//     log_exp("%d: Received SYN/ACK\n", id);
//     pthread_mutex_lock(&mutex_subconn);
//     subconn_infos[id].ini_seq_rem = ack;
//     pthread_mutex_unlock(&mutex_subconn);
//     ack++;
//     send_ACK(payload_sk, ack, seq, local_port);
//     log_exp("%d: Sent ACK and request\n", id);
//     seq += strlen(payload_sk);

//     //Wait for first data packet
//     int payload_len = wait_data(seq, ack, local_port, pkt_data_local);
    
//     //Send Optim Acks, pacing
//     log_exp("%d: Received first data, payload_len = %d, start optim ack\n", id, payload_len);
//     for (int i = 1; !nfq_stop; i++){
//         send_ACK("", ack+i*payload_len, seq, local_port);
//         usleep(ack_pacing);
//     }

// }

void* recv_idle(void* arg){
    int sock = (long) arg;
    int read_size = 0;
    char buffer[100];
    while (true){
        read_size = recv(sock , buffer , 100 , 0);
        if(read_size == 0)
        {
            log_exp("Client disconnected");
            break;
        }
        else if(read_size == -1)
        {
            // log("recv failed");
        }
        sleep(1);
    }
    log_exp("recv_idle exit");
    pthread_exit(NULL);
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
        log_exp("Receive client's request %d", read_size);
        for (int i = 0; i < SUBCONN_NUM; i++){
            int local_port = rand() % 20000 + 30000; 
            subconn_infos[i].local_port = local_port;//No nfq callback will interfere because iptable rules haven't been added
            subconn_infos[i].ini_seq_rem = 0;
            subconn_infos[i].ini_seq_loc = rand();
            subconn_infos[i].cur_seq_loc = subconn_infos[i].ini_seq_loc + 1 + read_size;
            subconn_infos[i].win_size = 29200*128;
            subconn_infos[i].ack_pacing = ack_pacing;
            subconn_infos[i].optim_ack_stop = 1;
            subconn_infos[i].mutex_opa = PTHREAD_MUTEX_INITIALIZER;
            log_exp("%d: local port = %d", i, local_port);

            // Add iptables rules
            generate_iptables_rules(iptable_rules, &iptable_rules_len, local_port);
            exec_iptables_rules(iptable_rules, iptable_rules_len-3, iptable_rules_len, 'A');
            log_exp("%d: iptables rules added", i);

            // Send SYN

            send_SYN("", 0, subconn_infos[i].ini_seq_loc, local_port);
            log_exp("%d: Sent SYN", i);
        }

        //Create idle recv thread
        pthread_t recv_thread;
        if (pthread_create(&recv_thread, NULL, recv_idle, (void *)(intptr_t)client_sock) != 0){
            log_error("Fail to create recv thread.");
            exit(EXIT_FAILURE);
        }
        break;
    }

    for(;;);

    nfq_stop = 1;

    cleanup();

    return 0;
}
