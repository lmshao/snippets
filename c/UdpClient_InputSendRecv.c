/*
 * Udp Client
 * 
 * int socket(int domain, int type, int protocol) 
 * int sendto(int socket, void *buffer, size_t size, int flags, struct sockaddr *to, size_t *addrlen)
 * int inet_aton(const char *cp, struct in_addr *inp)
 *
 * Copyright (c) 2016 Liming Shao <lmshao@163.com>
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#define DEFAULT_IP       "127.0.0.1"
#define DEFAULT_PORT     6666
#define BUFF_SIZE        200
int cliSocket;

void *KeepListening(){

    char buffer[BUFF_SIZE];

    for(;;){
        int n = recv(cliSocket, buffer, sizeof(buffer), 0);

        if(n < 0)
        {
            printf("recv err");
            continue;
        }
        else if(n == 0)
        {
            printf("\nDisconnect\n");
            break;
        }
        else
        {
            printf("Recv: %s\nlength: %d\n\n", buffer, n);
        }
        memset(buffer, 0, BUFF_SIZE);
    }
    return NULL;
}

int main(int argc, char *argv[])
{ 
    char *SERVER_IP = DEFAULT_IP;
    int SERVER_PORT = DEFAULT_PORT;
    pthread_t th;
    
    if(argc > 3)
        printf("param err:\nUsage: %s ip port | %s port | %s\n",argv[0],argv[0], argv[0]);
    else if(argc == 3)
    {
        SERVER_IP = argv[1];
        SERVER_PORT = atoi(argv[2]);
    }
    else if(argc == 2)
        SERVER_PORT = atoi(argv[1]);
    
    struct sockaddr_in servAddr;
    char sendbuf[BUFF_SIZE];
    
    if((cliSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf( "socket err");
        exit(1);
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(SERVER_PORT);
    inet_aton(SERVER_IP, &servAddr.sin_addr);
    
    
    printf("Send data to %s:%d ...\n\n", SERVER_IP, SERVER_PORT);
    
    pthread_create(&th, NULL, (void *(*)(void *)) KeepListening, NULL);

    while(fgets(sendbuf, BUFF_SIZE, stdin))
    {
        printf("Send: %s\n", sendbuf);
        int num = sendto(cliSocket, sendbuf, strlen(sendbuf), 0, (struct sockaddr *)&servAddr, sizeof(servAddr));
        if(num == strlen(sendbuf))
            printf("Send scuccess, length:%d\n\n",num);
        else
            printf("send err\n");
        
        memset(sendbuf, 0, BUFF_SIZE);
    }
    
    close(cliSocket);
    return 0;
}
