/*
 * Select Usage Sample 
 * int select (int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, const struct timeval *timeout)
 * Note:
 * As a tcp server, this program can accept multi-client connections simultaneously.
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
#include <sys/select.h>
#include <sys/time.h>

#define	LISTEN_NUM	5
#define BUFF_SIZE	200
#define DEFAULT_PORT	6666

int main(int argc, char *argv[])
{
	int SERVER_PORT = DEFAULT_PORT;
	
	if(argc > 2)
		printf("param err:\nUsage: %s port | %s\n",argv[0], argv[0]);
	if(argc == 2)
		SERVER_PORT = atoi(argv[1]);
	/*-----------------------------------------*/
	
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
	
    if(listen(servSocket, LISTEN_NUM) < 0)
    {
		printf("listen err");
		exit(1);
    }
	
	/*-----------------------------------------------------------------*/
	
	int	nready, client[FD_SETSIZE];
	fd_set fdset;
	
	int i, maxfd;
	int maxi = -1;					/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;			/* -1 indicates available entry */
	
	
	FD_ZERO(&fdset);	/* clear all bits in fdset */
	FD_SET(servSocket, &fdset);
	
	for(;;)
	{
		//maxfd = (fileno(fp) > sockfd ? fileno(fp) : sockfd);
		struct timeval timeout;
		timeout.tv_sec = 2;
		timeout.tv_usec = 0;
		
		int nready = select(maxfd+1, &fdset, NULL, NULL, &timeout);
		
		if(nready < 0)
		{
			printf("select err\n");
			break;
		}
		else if(nready == 0)
		{
			printf("select time out\n");
			continue;
		}
		else
		{
			if(FD_ISSET(servSocket, &fdset)) //socket is readable
			{
				socklen_t addrLen = sizeof(cliAddr);
				if((cliSocket = accept(servSocket, (struct sockaddr*)&cliAddr, &addrLen)) < 0)
				{
					printf("accept err");
					exit(1);
				}
			
				printf("\nConnect %s:%d\n", inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));
				
				for (i = 0; i < FD_SETSIZE; i++)
					if (client[i] < 0)
					{
						client[i] = cliSocket;	/* save descriptor */
						break;
					}
				
				if (i == FD_SETSIZE)
					perror("too many clients");
				
				FD_SET(cliSocket, &fdset);	/* add new descriptor to set */
				if (cliSocket > maxfd)
					maxfd = cliSocket;			/* for select */
				
				if (i > maxi)
					maxi = i;				/* max index in client[] array */

				if (--nready <= 0)
					continue;				/* no more readable descriptors */
			}
			
			for (i = 0; i <= maxi; i++) 	/* check all clients for data */
			{	
				if ( (cliSocket = client[i]) < 0)
					continue;
				
				if (FD_ISSET(cliSocket, &fdset)) 
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
						close(cliSocket);
						FD_CLR(cliSocket, &fdset);
						client[i] = -1;
					}
					else
					{
						printf("Recv: %s\nlength: %d\n\n", buffer, num);
					}
					if (--nready <= 0)
						break;				/* no more readable descriptors */
					
				}
			}
			
		}
	}
}


