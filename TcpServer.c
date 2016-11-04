/*
 * Tcp Server
 * 
 * Copyright (c) 2016 Liming Shao <lmshao@163.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <strings.h>

#define	LISTEN_NUM	5
#define BUFF_SIZE	200
#define SERVER_PORT	6666

int main()
{
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
	
	printf("Listen Port: 6666\nListening ...\n");
	
    if(listen(servSocket, LISTEN_NUM) < 0)
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
			int Num = recv(cliSocket, buffer, sizeof(buffer), 0);
		
			if(Num < 0)
			{
				printf("recv err");
				continue;
			}
			else if(Num == 0)
			{
				printf("Disconnect\n");
				break;
			}
			else
			{
				printf("Recv Data is %s\n", buffer);
			}

		}
	}
}