/*
 * Thread Synchronization - Mutex
 * 
 * int pthread_cond_init(pthread_cond_t *cond, const pthread_cond_attr *attr)
 * int pthread_cond_destory(pthread_cond_t *cond)
 * int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
 * int pthread_cond_timewait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime)
 * int pthread_cond_signal(pthread_cond_t *cond)
 * int pthread_cond_broadcast(pthread_cond_t *cond)
 *
 * Copyright (c) 2016 Liming Shao <lmshao@163.com>
 */
 
#include <stdio.h>
#include <pthread.h>

#define SIZE 4
#define OVER (-1)

struct producer
{
	int buffer[SIZE];
	int rpos, wpos;
	pthread_mutex_t lock;
	pthread_cond_t notempty;
	pthread_cond_t notfull;
};

struct producer buffer;

void Init(struct producer *b)
{
	pthread_mutex_init(&b->lock, NULL);		//互斥锁
	pthread_cond_init(&b->notempty, NULL);	//条件变量
	pthread_cond_init(&b->notfull, NULL);	//条件变量
	b->rpos = 0;
	b->wpos = 0;
}

void Put(struct producer *b, int data)
{
	pthread_mutex_lock(&b->lock);	//lock
	
	while((b->wpos+1)%SIZE == b->rpos)	//当缓冲区满时：将要写的位置
		pthread_cond_wait(&b->notfull, &b->lock);	//等待notfull信号
	
	b->buffer[b->wpos++] = data;

	if(b->wpos >= SIZE)
		b->wpos = 0;
	
	pthread_cond_signal(&b->notempty);	//发送notempty信号
	pthread_mutex_unlock(&b->lock);
}

int Get(struct producer *b)
{
	pthread_mutex_lock(&b->lock);
	
	while(b->wpos == b->rpos)	//当缓冲区空时
		pthread_cond_wait(&b->notempty, &b->lock);	//等待notempty信号
	
	int data = b->buffer[b->rpos++];

	if(b->rpos >= SIZE) 
		b->rpos = 0;
	
	pthread_cond_signal(&b->notfull);	//发送notfull信号
	pthread_mutex_unlock(&b->lock);
	
	return data;
}

void *Producer()
{
	int n;
	for(n=0; n<20; n++)
	{
		printf("Producer:%d-->\n",n);
		Put(&buffer, n);
	}
	Put(&buffer, OVER);
}

void *Consumer()
{
	for(;;)
	{
		int d = Get(&buffer);
		if(d == OVER)	
			break;
		printf("Consumer:-->%d\n", d);
	}
}

int main()
{
	pthread_t tha, thb;
	void *retval;
	Init(&buffer);
	pthread_create(&tha, NULL, Producer, 0);
	pthread_create(&thb, NULL, Consumer, 0);
	pthread_join(tha, &retval);
	pthread_join(thb, &retval);
	return 0;
}
