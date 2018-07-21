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
#include <string.h>
#include <unistd.h>

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/select.h>
#include <sys/time.h>

#define BACKLOG         5
#define BUFF_SIZE     200
#define DEFAULT_PORT 6666

typedef struct _CLIENT  
{  
    int fd;        /* client's connection descriptor */  
    struct sockaddr_in addr;    /* client's address */
}CLIENT;

int main(int argc, char *argv[])
{
    int SERVER_PORT = DEFAULT_PORT;
    
    if(argc > 2)
        printf("param err:\nUsage:\n\t%s port | %s\n\n",argv[0], argv[0]);
    if(argc == 2)
        SERVER_PORT = atoi(argv[1]);
    
    int i, maxi, maxfd, nready, nbytes;
    int servSocket, cliSocket;
    fd_set allset, rset;
    socklen_t addrLen;
    char buffer[BUFF_SIZE];
    CLIENT client[FD_SETSIZE];        /* client info */
    struct sockaddr_in servAddr, cliAddr;
    
    if((servSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket err");
        exit(1);
    }

    bzero(&servAddr,sizeof(servAddr));
    servAddr.sin_family      = AF_INET;
    servAddr.sin_port        = htons(SERVER_PORT);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if(bind(servSocket,(struct sockaddr *)&servAddr,sizeof(servAddr)) < 0)
    {
        printf("bind err");
        exit(1);
    }
    
    if(listen(servSocket, BACKLOG) < 0)
    {
        printf("listen err");
        exit(1);
    }
    printf("Listen Port: %d\nListening ...\n", SERVER_PORT);
    
    maxi = -1;
    maxfd = servSocket;
    for (i = 0; i < FD_SETSIZE; i++)
        client[i].fd = -1;            /* -1 indicates available entry */
    
    FD_ZERO(&allset);    /* clear all bits in allset */
    FD_SET(servSocket, &allset);
    for( ;; )
    {
        rset= allset;
/*
        struct timeval timeout;
        timeout.tv_sec = 2;
        timeout.tv_usec = 0;
*/        
        if((nready = select(maxfd+1, &rset, NULL, NULL, NULL)) < 0)
        {
            printf("select err\n");
            break;
        }
        else if(nready == 0)    /* when timeout is set. */
        {
            printf("select time out\n");
            continue;
        }

        if(FD_ISSET(servSocket, &rset))        /* new client connections */
        {
            addrLen = sizeof(cliAddr);
            if((cliSocket = accept(servSocket, (struct sockaddr*)&cliAddr, &addrLen)) < 0)
            {
                printf("accept err");
                exit(1);
            }
            
            printf("\nNew client connections %s:%d\n", inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));
            
            for (i = 0; i < FD_SETSIZE; i++)
                if (client[i].fd < 0)    /* save client info */
                {
                    client[i].fd = cliSocket;
                    client[i].addr = cliAddr;
                    break;
                }
            
            if (i == FD_SETSIZE)
                perror("too many clients");
            
            FD_SET(cliSocket, &allset);    /* add new descriptor to set */
            if (cliSocket > maxfd)
                maxfd = cliSocket;        /* for select */
            
            if (i > maxi)
                maxi = i;                /* max index in client[] array */

            if (--nready <= 0)
                continue;                /* no more readable descriptors */
        }

        for (i = 0; i <= maxi; i++)        /* check all clients for data */
        {    
            if ((cliSocket = client[i].fd) < 0)
                continue;
            
            if (FD_ISSET(cliSocket, &rset)) 
            {
                memset(buffer, 0, BUFF_SIZE);
                if((nbytes = recv(cliSocket, buffer, sizeof(buffer), 0)) < 0)
                {
                    printf("recv err");
                    continue;
                }
                else if(nbytes == 0)
                {
                    printf("\nDisconnect %s:%d\n", 
                        inet_ntoa(client[i].addr.sin_addr), ntohs(client[i].addr.sin_port));
                    close(cliSocket);
                    FD_CLR(cliSocket, &allset);
                    client[i].fd = -1;
                }
                else
                {
                    printf("\nFrom %s:%d\n", 
                        inet_ntoa(client[i].addr.sin_addr), ntohs(client[i].addr.sin_port));
                    printf("Recv: %sLength: %d\n\n", buffer, nbytes);
                }
                if (--nready <= 0)
                    break;        /* no more readable descriptors */
            }
        }
    }
}


