#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

void mysleep(unsigned int msec)
{
    struct timespec delay, remain;
    delay.tv_sec = msec / 1000;
    delay.tv_nsec = (msec % 1000) * 1000000ll;

    int ret = nanosleep(&delay, &remain);

    while (ret != 0)
    {
        delay = remain;
        ret = nanosleep(&delay, &remain);
    }
}



int main(int argc, char *argv[])
{ 
	int SERVER_PORT = 5555;
	char * SERVER_IP = "127.0.0.1";
	
	if(argc == 3){
		SERVER_IP = argv[1];
		SERVER_PORT = atoi(argv[2]);
	}else if(argc == 2){
		SERVER_PORT = atoi(argv[1]);
	}else ;
		
	
	printf("dstIP: %s ,dstPort: %d\n",SERVER_IP, SERVER_PORT);
	
    struct sockaddr_in serverAddr;
    int clientSocket;
    char sendbuf[200];	//send traffic: 200+56 = 256 Bytes = 2KB; 72+56 = 128 B = 1KB.
    if((clientSocket=socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
	perror( "socket error" );
	return -1;
    }

    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr=inet_addr(SERVER_IP);
    
    if(connect(clientSocket,(struct sockaddr *)&serverAddr,sizeof(serverAddr)) < 0)
    {
	perror("connect");
	exit(1);
    }

    printf("connect with destination host.....\n");
   
   	memset(sendbuf,'a',200*sizeof(char));
	
   	int n;
	long long L1,L2;
	struct timeval tv;
    while(1)
    {
		gettimeofday(&tv,NULL);
		L1 = tv.tv_sec*1000*1000 + tv.tv_usec;
		
		int i;
		mysleep(15);				// 15 + 10 ~= 1 Mb/s
		for(n=0; n < 10; n++)
		{
			i = send(clientSocket,sendbuf,sizeof(sendbuf),0);
			printf("send %d Bytes\n", (i+56)*8 );
		}
		
		
		gettimeofday(&tv,NULL);
		L2 = tv.tv_sec*1000*1000 + tv.tv_usec;
		printf("%lld \n",L2-L1);
    }
    close(clientSocket);
    return 0;
}
