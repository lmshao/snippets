/*
 * ICMP - Ping tool
 * 
 * Create ICMP RAW socket
 * Send & receive ICMP message
 *
 * Copyright (c) 2018 Liming Shao <lmshao@163.com>
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <strings.h>

#include <signal.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
// #include <sys/select.h>

int sockfd;
pid_t pid;    /* our PID */
int dataLen = 56;
struct sockaddr *sarecv;
struct sockaddr *sasend;
socklen_t salen;
int nsent;

struct pingstat{
    int nsent;
    int nrecv;
    double minTime;
    double maxTime;
    double avgTime;
};

static struct pingstat pingStat = {0, 0, 0, 0, 0};
static char *hostname = "lmshao.com";

void icmpProc(char *ptr, ssize_t len, struct msghdr *msg, struct timeval *tvrecv){
    int hlen1, icmpLen;
    double rtt;
    struct ip *ipHeader;
    struct icmp *icmpHeader;
    struct timeval *tvsend;
    char *ip;

    ipHeader = (struct ip*) ptr; /*start of IP header*/
    hlen1 = ipHeader->ip_hl << 2; /* length of IP header */
    if (ipHeader->ip_p != IPPROTO_ICMP)
        return;

    icmpHeader = (struct icmp *)(ptr+hlen1);
    icmpLen = (int)(len - hlen1);
    if (icmpLen < 8)
        return;

    if (icmpHeader->icmp_type == ICMP_ECHOREPLY) {
        if (icmpHeader->icmp_id != pid)
            return;
        
        if (icmpLen < 16)
            return;

        tvsend = (struct timeval*)icmpHeader->icmp_data;
        timersub(tvrecv, tvsend, tvrecv);
        rtt = tvrecv->tv_sec * 1000.0 + tvrecv->tv_usec / 1000.0;

        ip = inet_ntoa(((struct sockaddr_in *)sarecv)->sin_addr);

        printf("%d bytes from %s: seq=%u, ttl=%d, rtt=%.3f ms\n",
               icmpLen, ip, icmpHeader->icmp_seq, ipHeader->ip_ttl, rtt);
        
        if (pingStat.minTime == 0) 
            pingStat.minTime = rtt;
        
        pingStat.nrecv++;
        pingStat.minTime = pingStat.minTime < rtt ? pingStat.minTime : rtt;
        pingStat.maxTime = pingStat.maxTime > rtt ? pingStat.maxTime : rtt;
        pingStat.avgTime = ((pingStat.avgTime * (pingStat.nrecv - 1)) + rtt) / pingStat.nrecv;
    }
}

uint16_t calChecksum(void *addr, int len)
{
    int nleft = len;
    uint32_t sum = 0;
    uint16_t *w = addr;

    /*
    * Our algorithm is simple, using a 32 bit accumulator (sum), we add
    * sequential 16 bit words to it, and at the end, fold back all the
    * carry bits from the top 16 bits into the lower 16 bits.
    */
    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }

    /* mop up an odd byte, if necessary */
    if (nleft == 1) {
        sum += *(unsigned char *)w;
    }

    /* add back carry outs from top 16 bits to low 16 bits */
    while (sum >> 16) {
        sum = (sum >> 16) + (sum & 0xffff);    /* add hi 16 to low 16 */
    }

    /* truncate to 16 bits */
    return (uint16_t)(~sum); 
}

void icmpSend(){
    int len;
    char sendbuf[100];
    struct icmp *icmpHeader;
    char *pos;

    /*
    *        IMCP Message Format
    *  
    *  0       8       16      24      32        
    *  +-------+-------+-------+-------+
    *  |  Type |  Code |    Checksum   |
    *  +-------+-------+---------------+
    *  |  Identifier   |  Sequence No. |
    *  +---------------+---------------+
    *  | Optional Data (Timestamp+no.) ?
    *  +-------------------------------+
    */

    icmpHeader = (struct icmp*)sendbuf;
    icmpHeader->icmp_type = ICMP_ECHO;
    icmpHeader->icmp_code = 0;
    icmpHeader->icmp_id = pid;
    icmpHeader->icmp_seq = nsent++;
    pos = icmpHeader->icmp_data;

    for (int i = 0; i < dataLen; i++){
        *pos++ = i;
    }

    gettimeofday((struct timeval *)icmpHeader->icmp_data, NULL);

    len = 8 + dataLen;  /* checksum ICMP header and data */
    icmpHeader->icmp_cksum = 0;
    icmpHeader->icmp_cksum = calChecksum(icmpHeader, len);

    ssize_t nbytes = sendto(sockfd, sendbuf, len, 0, sasend, salen);
    if (nbytes != len)
        perror("sendto");
    else
        pingStat.nsent++;
}

void sigAlrm(int signo){
    icmpSend();
    alarm(1);
    return;
}

void sigInt(int signo) {
    printf("\n--- %s ping statistics ---\n", hostname);
    printf("%d packets transmitted, %d packets received, %.1f%% packet loss\n", 
        pingStat.nsent, pingStat.nrecv, (double)((pingStat.nsent - pingStat.nrecv)*100)/pingStat.nsent);
    printf("round-trip min/avg/max = %.3f/%.3f/%.3f ms\n", pingStat.minTime, pingStat.avgTime, pingStat.maxTime);
    exit(0);
}

void readLoop() {
    int size;
    char recvbuf[100];
    char controlbuf[CMSG_SPACE(64)];
    struct iovec iov;
    struct msghdr msg;
    ssize_t n;
    struct timeval tval;
    // int nready;
    // fd_set rset;
    // struct timeval timeout;

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd <=0 ){
        perror("socket");
        return;
    }
    setuid(getuid()); /* don't need special permissions any more */

    size = 60 * 1024;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));

    sigAlrm(SIGALRM);

    iov.iov_base = recvbuf;
    iov.iov_len = sizeof(recvbuf);
    msg.msg_name = sarecv;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = controlbuf;

    // FD_ZERO(&rset);
    // FD_SET(sockfd, &rset);

    // timeout.tv_sec = 1;
    // timeout.tv_usec = 0;

    for (;;) {
        msg.msg_namelen = salen;
        msg.msg_controllen = sizeof(controlbuf);

        // nready = select(sockfd+1, &rset, NULL, NULL, &timeout);
        // if (nready < 0){
        //     /* Ignore interrupted system call - SIGALRM */
        //     if (errno == EINTR)
        //         continue;
            
        //     perror("select");
        //     break;
        // } else if (nready == 0) {
        //     printf("Request timeout for icmp_seq\n");
        //     continue;
        // }

        n = recvmsg(sockfd, &msg, 0);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            else
                printf("recvmsg error.\n");
        }

        gettimeofday(&tval, NULL);
        icmpProc(recvbuf, n, &msg, &tval);
    }
}

int main(int argc, char *argv[])
{
    struct addrinfo *ai;
    // char *host = "lmshao.com";
    char *ip;
    pid = getpid() & 0xffff;
    int res = 0;

    if (argc == 2){
        hostname = argv[1];
    } else if (argc > 2) {
        printf("Usage: %s hostname\n", argv[0]);
        return 1;
    }

    signal(SIGALRM, sigAlrm);
    signal(SIGINT, sigInt);

    struct addrinfo hints;
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = AF_INET;

    res = getaddrinfo(hostname, NULL, &hints, &ai);
    if (res != 0) {
        printf("getaddrinfo: %s\n", gai_strerror(res));
        return 1;
    }

    /* Binary -> ASCII */
    ip = inet_ntoa(((struct sockaddr_in *)ai->ai_addr)->sin_addr);

    printf("PING %s (%s): %d data bytes\n", 
            ai->ai_canonname ? ai->ai_canonname : ip, ip, dataLen);

    sasend = ai->ai_addr;
    sarecv = calloc(1, ai->ai_addrlen);
    salen = ai->ai_addrlen;

    readLoop();
    return 0;
}