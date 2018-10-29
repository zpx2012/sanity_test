#include <linux/tcp.h>
#include <linux/ip.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int main(int argc , char *argv[])
{
    int sock, bytes = 0, i, client_socklen;
    struct sockaddr_in server, client;
    struct timeval pkt_this_tv, pkt_last_tv, pkt_intvl_tv;
    struct timeval ses_this_tv, ses_last_tv, ses_intvl_tv;
    char client_message[2000];
    char message[] = "GET /sdk-tools-linux-3859397.zip HTTP/1.1\r\nHost: 169.235.31.181\r\nConnection: keep-alive\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Safari/537.36\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\nReferer: http://169.235.31.181/\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: zh-CN,zh;q=0.9,en;q=0.8\r\n";

    char* ip = argv[1];

    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        perror("Could not create socket\n");
        return -1;
    }
    printf("Socket created\n");

    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(80);
 
    //Connect to remote server
    while (connect(sock, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) < 0)
    {
        perror("connect failed. Retry");
       
    }     
    printf("Connected\n");     

    if((bytes = send(sock, message, strlen(message)+1, MSG_DONTWAIT)) < 0) {
        perror("Error on sendto()");
        return -1;
    }
    printf("Success! Sent %d bytes.\n", bytes);

    //Receive a message from client
    while( (bytes = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Send the message back to client
        printf("Received %d bytes.\n", bytes);
    }

    if(bytes == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(bytes == -1)
    {
        perror("recv failed");
    }
    
    close(sock);

    return 0;
}