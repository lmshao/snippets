/*
 * epoll Usage Sample
 * int epoll_create (int __size);
 * int epoll_create1 (int __flags);
 * int epoll_ctl (int __epfd, int __op, int __fd, struct epoll_event *__event);
 * int epoll_wait (int __epfd, struct epoll_event *__events, int __maxevents, int __timeout);
 *
 * Note:
 * As a tcp server, this program can accept multi-client connections simultaneously.
 *
 * Copyright (c) 2020 Liming Shao <lmshao@163.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/epoll.h>

#define BACKLOG 5
#define BUFF_SIZE 200
#define DEFAULT_PORT 6666
#define MAX_EVENTS 10

int main(int argc, char *argv[])
{
    int SERVER_PORT = DEFAULT_PORT;

    if (argc > 2)
        printf("param err:\nUsage:\n\t%s port | %s\n\n", argv[0], argv[0]);
    if (argc == 2)
        SERVER_PORT = atoi(argv[1]);

    int nbytes;
    char buffer[BUFF_SIZE];

    int servSocket, cliSocket;
    socklen_t addrLen;
    struct sockaddr_in servAddr, cliAddr;

    struct epoll_event ev, readyEvents[MAX_EVENTS];
    int nfds, epollfd;

    if ((servSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket err");
        exit(1);
    }

    int optval = 1;
    if (setsockopt(servSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("setsockopt");
        exit(0);
    }

    bzero(&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(SERVER_PORT);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(servSocket, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(servSocket, BACKLOG) < 0) {
        perror("listen");
        exit(1);
    }
    printf("Listen Port: %d\nListening ...\n", SERVER_PORT);

    /* Create an epoll instance */
    if ((epollfd = epoll_create1(0)) == -1) {
        perror("epoll_create");
        exit(1);
    }

    ev.events = EPOLLIN;
    ev.data.fd = servSocket;
    /* Add an event of interest*/
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, servSocket, &ev) == -1) {
        perror("epoll_ctl");
        exit(1);
    }

    for (;;) {
        /* Wait for events on an epoll instance "epfd"
         * Returns the number of triggered events returned in "events" buffer.
         */
        if ((nfds = epoll_wait(epollfd, readyEvents, MAX_EVENTS, -1)) == -1) {
            perror("epoll_wait");
            exit(1);
        }

        for (int n = 0; n < nfds; n++) {
            if (readyEvents[n].data.fd == servSocket) {
                cliSocket = accept(servSocket, (struct sockaddr *)&cliAddr, &addrLen);
                if (cliSocket == -1) {
                    perror("accept");
                    exit(1);
                }

                printf("\nNew client connections client[%d] %s:%d\n", cliSocket,
                       inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));

                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = cliSocket;
                /* Add a new event of interest*/
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, cliSocket, &ev) == -1) {
                    perror("epoll_ctl: cliSocket");
                    exit(1);
                }
            } else {
                cliSocket = readyEvents[n].data.fd;

                memset(buffer, 0, BUFF_SIZE);
                /* recv data */
                nbytes = recv(cliSocket, buffer, sizeof(buffer), 0);
                if (nbytes < 0) {
                    perror("recv");
                    continue;
                } else if (nbytes == 0) {
                    printf("\nDisconnect fd[%d]\n", cliSocket);
                    close(cliSocket);
                } else {
                    printf("\nFrom fd[%d]", cliSocket);
                    printf("\nRecv: %sLength: %d\n\n", buffer, nbytes);
                }
            }
        }
    }

    //    return 0;
}
