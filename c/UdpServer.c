/*
 * Udp Server
 * 
 * int socket(int domain, int type, int protocol)
 * int bind(int sockfd, struct sockaddr *myaddr, int addrlen)
 * int recvfrom(int socket, void *buffer, size_t size, int flags, struct sockaddr *from, size_t *addrlen)
 * char *inet_ntoa(struct in_addr in)
 *
 * Note:
 * As a udp server, this program can accept multi-client connections simultaneously.
 * 
 * Copyright (c) 2016 Liming Shao <lmshao@163.com>
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define BUFF_SIZE    200
#define DEFAULT_PORT    6666

int main(int argc, char *argv[])
{
    int SERVER_PORT = DEFAULT_PORT;
    
    if(argc > 2)
        printf("param err:\nUsage: %s port | %s\n",argv[0], argv[0]);
    if(argc == 2)
        SERVER_PORT = atoi(argv[1]);        
    
    int servSocket, nbytes;
    struct sockaddr_in servAddr, cliAddr;
    int addrLen = sizeof(cliAddr);
    
    char buffer[BUFF_SIZE];

    if((servSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("socket err");
        exit(1);
    }

    bzero(&servAddr,sizeof(servAddr));
    
    servAddr.sin_family    = AF_INET;
    servAddr.sin_port = htons(SERVER_PORT);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if(bind(servSocket,(struct sockaddr *)&servAddr,sizeof(servAddr)) < 0)
    {
        printf("bind err");
        exit(1);
    }
    
    printf("Listen Port: %d\nListening ...\n", SERVER_PORT);
    
    while(1)
    {
        if((nbytes = recvfrom(servSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliAddr, &addrLen)) < 0)
        {
             printf("recvfrom err");
             exit(1);
        }
        
        printf("\nFrom %s:%d\n", inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));
        printf("Recv: %s\nlength: %d\n\n", buffer, nbytes);
        memset(buffer, 0, BUFF_SIZE);
    }
}