/*
 * Tcp Server
 * 
 * int socket(int domain, int type, int protocol)
 * int bind(int sockfd, struct sockaddr *myaddr, int addrlen)
 * int listen(int sockfd, int backlog)
 * int accept(int sockfd, struct sockaddr *addr, int *addrlen)
 * int recv(int socket, void *buffer, int length, int flag)
 * char *inet_ntoa(struct in_addr in) 
 *
 * Copyright (c) 2016 Liming Shao <lmshao@163.com>
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

#define	BACKLOG	5
#define BUFF_SIZE	200
#define DEFAULT_PORT	6666

int main(int argc, char *argv[])
{
	int SERVER_PORT = DEFAULT_PORT;
	
	if(argc > 2)
		printf("param err:\nUsage: %s port | %s\n",argv[0], argv[0]);
	if(argc == 2)
		SERVER_PORT = atoi(argv[1]);		
	
    int servSocket, cliSocket;
    struct sockaddr_in servAddr, cliAddr;
	socklen_t addrLen = sizeof(cliAddr);
	char buffer[BUFF_SIZE];

    if((servSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
		printf("socket err");
		exit(1);
    }

    bzero(&servAddr,sizeof(servAddr));
    
    servAddr.sin_family	= AF_INET;
    servAddr.sin_port = htons(SERVER_PORT);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if(bind(servSocket,(struct sockaddr *)&servAddr,sizeof(servAddr)) < 0)
    {
		printf("bind err");
		exit(1);
    }
	
	printf("Listen Port: %d\nListening ...\n", SERVER_PORT);
	
    if(listen(servSocket, BACKLOG) < 0)
    {
		printf("listen err");
		exit(1);
    }
	
    while(1)
    {
        if((cliSocket = accept(servSocket, (struct sockaddr*)&cliAddr, &addrLen)) < 0)
        {
	     	printf("accept err");
	     	exit(1);
        }
		
		printf("\nConnect from %s:%d\n", inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));
		
		while(1)
		{
			int num = recv(cliSocket, buffer, sizeof(buffer), 0);
		
			if(num < 0)
			{
				printf("recv err");
				continue;
			}
			else if(num == 0)
			{
				printf("\nDisconnect %s:%d\n", inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));
				break;
			}
			else
			{
				printf("Recv: %s\nlength: %d\n\n", buffer, num);
			}
			memset(buffer, 0, BUFF_SIZE);

		}
	}
}