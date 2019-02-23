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
#include <time.h>
#include <sys/time.h>

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

double timeval2sec(struct timeval* tv)
{
    return tv->tv_sec + (0.000001 * tv->tv_usec);
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
// FILE * fp;
void intHandler(int dummy){
    printf("Catch Ctrl+C\n");
    // fclose(fp);
}



int main(int argc , char *argv[])
{
    int sock, bytes = 0, i, client_socklen;
    // struct timeval pkt_this_tv, pkt_last_tv, pkt_intvl_tv;
    // struct timeval ses_this_tv, ses_last_tv, ses_intvl_tv;
    char client_message[2000],*msg,*tstr;
    char req_str[1448] = "GET /sdk-tools-linux-3859397.zip HTTP/1.1\r\nHost: 169.235.31.181\r\nConnection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\nReferer: http://169.235.31.181/\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: zh-CN,zh;q=0.9,en;q=0.8\r\n";
    char trick_str[] = "GET ";
    char space_str[1448];
    struct timeval ses_this_tv, ses_last_tv, ses_intvl_tv,pkt_last_tv,pkt_intvl_tv,fpkt_intvl_tv;
    struct timespec fpkt_intvl_ts;
    double speed,intvl;

    for(i=0;i<1448;i++)
        space_str[i] = ' ';
    unsigned int trick_str_len = strlen(trick_str), space_str_len = strlen(space_str);

    if(argc < 4){
        perror("At least 2 arguments.");
        return -1;
    }
    uint32_t dstip_u32 = inet_addr(argv[1]);
    int dport = atoi(argv[2]);
    int sport = atoi(argv[3]);
    // char* ofile = argv[4];
    // printf("%s\n",ofile);

    srand(time(0));
    seed = rand() % 65536;

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

    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT, intHandler);
    i = 0;
    if(send(sock,trick_str,trick_str_len,0) < 0){
        perror("Error on sendto()");
        return -1;
    }
    // fp = fopen(ofile, "a+");
    // if(fp == NULL)
    // {
    //   printf("Error!");   
    //   exit(1);             
    // }
    gettimeofday(&ses_last_tv, NULL);
    gettimeofday(&pkt_last_tv,NULL);
    fpkt_intvl_ts.tv_sec = 0;
    fpkt_intvl_ts.tv_nsec = 188541;
    fpkt_intvl_tv.tv_sec = 0;
    fpkt_intvl_tv.tv_usec = fpkt_intvl_ts.tv_nsec/1000;    
    while(1){        
            gettimeofday(&ses_this_tv, NULL);
            timersub(&ses_this_tv,&ses_last_tv,&ses_intvl_tv);
            intvl = timeval2sec(&ses_intvl_tv);
            if(intvl > 1.0){
                speed = (trick_str_len + space_str_len * i) / intvl / 1024.0;
                // print_utc_time((time_t)ses_this_tv.tv_sec);               
                printf("Thrput: %f KB/s\n",speed);
                // tstr = asctime(gmtime(&ses_this_tv.tv_sec));
                // tstr[strlen(tstr)-1] = 0;
                // fprintf(fp,"%s UTC, %f KB/s\n",tstr,speed);
                // fflush(fp);
                // printf("%s UTC, %f KB/s\n",tstr,speed);                
                i = 0;
                if(speed > 300)
                    sleep(300);
                gettimeofday(&ses_last_tv,NULL);
            }
            if(!reach_interval(&ses_this_tv,&pkt_last_tv,&fpkt_intvl_tv)){
                nanosleep(&fpkt_intvl_ts,NULL);
                printf("nanosleen");
            }
            if(send(sock,space_str,space_str_len,0) < 0){
                perror("Error on sendto()");
                break;
            }
            i++;
            gettimeofday(&pkt_last_tv,NULL);
            printf("The current local time is: %ld.%06ld\n",pkt_last_tv.tv_sec,pkt_last_tv.tv_usec);
        // printf("Success! Sent %d bytes.\n", bytes);
    }
    printf("out of loop\n");
    // fclose(fp);

    return 0;
}