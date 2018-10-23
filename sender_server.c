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

int reach_interval(struct timeval* thistime,struct timeval* lasttime,struct timeval* intervaltime){
    return thistime->tv_sec > lasttime->tv_sec + intervaltime->tv_sec
            || (thistime->tv_sec == lasttime->tv_sec + intervaltime->tv_sec
                && thistime->tv_usec >= lasttime->tv_usec + intervaltime->tv_usec);
}

void set_timeval(struct timeval* tv,const float ftime){
    int mpler = 1000000;
    int dt = ftime * mpler;
    tv->tv_sec =  dt/mpler;
    tv->tv_usec = dt%mpler; 
}

int main(int argc , char *argv[])
{
    int sock, client_sock, readsize, bytes = 0, i, client_socklen;
    struct sockaddr_in server, client;
    struct timeval pkt_this_tv, pkt_last_tv, pkt_intvl_tv;
    struct timeval ses_this_tv, ses_last_tv, ses_intvl_tv;
    char client_message[2000];
    char message[] = "GET /sdk-tools-linux-3859397.zip HTTP/1.1\r\nHost: 169.235.31.181\r\nConnection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\nReferer: http://169.235.31.181/\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: zh-CN,zh;q=0.9,en;q=0.8\r\npadingpadingpadingpadingpadingpadingpadingpading\n";
    
    float pkt_intvl_array[5] = {0.001,0.01,0.1,1,10};
    int max_pkt_intvl_index = 5;
//    float pkt_intvl_array[12] = {0.001,0.004,0.007,0.01,0.04,0.07,};
    
    set_timeval(&ses_intvl_tv,600.0);

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        perror("Could not create socket\n");
        return -1;
    }
    printf("Socket created\n");
     
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(80);
     
    //Bind
    if( bind(sock,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
    puts("bind done");
     
    //Listen
    listen(sock, 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    client_socklen = sizeof(struct sockaddr_in);
     
    //accept connection from an incoming client
    client_sock = accept(sock, (struct sockaddr *)&client, (socklen_t*)&client_socklen);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");

    if(recv(client_sock , client_message , 2000 , 0) < 0){
        perror("recv failed.");
        return -1;
    }
    puts("Received msg");

    gettimeofday(&ses_last_tv, NULL);
    printf("The current local time is: %ld.%06ld\n",ses_last_tv.tv_sec,ses_last_tv.tv_usec);
    for(i = 0; i < max_pkt_intvl_index; i++){
        set_timeval(&pkt_intvl_tv,pkt_intvl_array[i]);
        gettimeofday(&ses_this_tv, NULL);
        while(!reach_interval(&ses_this_tv,&ses_last_tv,&ses_intvl_tv)){
            gettimeofday(&pkt_this_tv, NULL);
            if(reach_interval(&pkt_this_tv,&pkt_last_tv,&pkt_intvl_tv)){
                pkt_last_tv = pkt_this_tv;
                if((bytes = send(client_sock, message, strlen(message)+1, MSG_DONTWAIT)) < 0) {
                    perror("Error on sendto()");
                    return -1;
                }
                printf("Success! Sent %d bytes.\n", bytes);
            }
            gettimeofday(&ses_this_tv,NULL);
        }
        ses_last_tv = ses_this_tv;
        printf("reached session interval\n");
        printf("The current local time is: %ld.%06ld\n",ses_this_tv.tv_sec,ses_this_tv.tv_usec);
        break;
    }
         
    close(sock);
    return 0;
}

//        printf("The current local time is: %ld.%06ld\n",tv.tv_sec,tv.tv_usec);