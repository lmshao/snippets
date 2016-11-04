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
#include <pthread.h>
#include <sys/time.h>

#define SERVER_PORT 5555

/*用于给线程传递参数*/
struct mystr
{
    char *ip;
    int port;
    int client;
};

void recvdata();

int main()
{
    int client;
    int serverSocket;
    pthread_t thread;
    struct mystr info;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    int addr_len = sizeof(clientAddr);

    if((serverSocket = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
		perror("socket");
		exit(1);
    }

    bzero(&serverAddr,sizeof(serverAddr));
    
    serverAddr.sin_family =AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if(bind(serverSocket,(struct sockaddr *)&serverAddr,sizeof(serverAddr)) < 0)
    {
		perror("connect");
		exit(1);
    }

    if(listen(serverSocket,5)<0)
    {
		perror("listen");
		exit(1);
    }
    while(1)
    {
        if((client = accept(serverSocket,(struct  sockaddr*)&clientAddr,(socklen_t*)&addr_len)) < 0)
        {
	     	perror("accept");
	     	exit(1);
        }
		
	info.ip = inet_ntoa(clientAddr.sin_addr);
	info.port = htons(clientAddr.sin_port);
	info.client = client;
	printf("\nAccept %s:%d\n\n", info.ip, info.port);
    pthread_create(&thread,NULL,(void *)recvdata,&info);
    }
}

void recvdata(void *arg)
{
    struct mystr *info;
    info = (struct mystr *)arg;
    int retval,Num;
    char buffer[1024];
	
	struct timeval tv_now, tv_last, tv_begin;
	gettimeofday(&tv_now, NULL);
	tv_last = tv_now;
	tv_begin = tv_now;
	
	float allData = 0, validData = 0;
	long allDataSum = 0, validDataSum=0;
	int n;
	
    while(1)
    {
		gettimeofday(&tv_now, NULL);
		int one = (int) (tv_now.tv_sec - tv_last.tv_sec);
		if( one > 0)
		{
			allData *=8;
			validData *=8;
			
			if(allData >= 1024)
				printf("\nReal-time Traffic: %.2f Kb/s, %.0f b/s, %d, Payload: %.2f Kb/s \n", allData/(1024*one), allData/one, n/one, validData/(1024*one) );
			else
				printf("\nReal-time Traffic: %f b/s, %d, Payload: %f b/s \n",allData/one, n/one, validData/one );
			
			allDataSum += allData;
			validDataSum += validData;
			
			int m = (int)(tv_now.tv_sec - tv_begin.tv_sec);
			printf("Average Traffic: %ld Kb/s, Payload: %ld Kb/s\n, ", allDataSum/(1024*m), validDataSum/(1024*m));
			
			allData = 0;
			validData = 0;
			
			n=0;
			tv_last = tv_now;
		}
		
		Num = recv(info->client,buffer,sizeof(buffer),0);
		
		if(Num < 0)
		{
	    	perror("Recv");
	    	continue;
		}
		else if(Num == 0)
		{
			printf("disconnect %s\n",info->ip);
			break;
		}
		else
		{
			// printf("from Address: %s:%d, ",info->ip,info->port);
			allData += (Num+56);
			validData += Num;
			n++;
			// printf("%d ", (iDataNum+56));
		    // printf("Recv Data is %s\n",buffer);
		}
    }
    pthread_exit(&retval);
}
