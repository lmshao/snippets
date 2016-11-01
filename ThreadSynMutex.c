/*
 * Thread Synchronization - Mutex
 * 
 * int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutex_attr *attr)
 * int pthread_mutex_destory(pthread_mutex_t *mutex)
 * int pthread_mutex_lock(pthread_mutex_t *mutex)
 * int pthread_mutex_trylock(pthread_mutex_t *mutex)
 * int pthread_mutex_unlock(pthread_mutex_t *mutex)
 * int pthread_mutex_destory(pthread_mutex_t *mutex)
 * 
 * Copyright (c) 2016 Liming Shao <lmshao@163.com>
 */
 
#include <stdio.h>
#include <pthread.h>

int count = 0, mark = 0;
pthread_mutex_t mutex;

void Func()
{
	for(; ;)
	{
		pthread_mutex_lock(&mutex);
		if(mark)
		{	
			printf("Thread A: \t%d\n",count++);
			mark = 0;
			sleep(1);
		}
		pthread_mutex_unlock(&mutex);
	}
}

int main()
{
	pthread_t p;
	pthread_mutex_init(&mutex, NULL);
	pthread_create(&p, NULL, (void *)&Func, NULL);
	for(;;)
	{
		pthread_mutex_lock(&mutex);
		
		if(!mark)
		{
			printf("Thread B: %d\n", count++);
			mark = 1;
			sleep(1);
		}
		
		pthread_mutex_unlock(&mutex);
	}
	/*
	pthread_mutex_destory(&mutex);
	*/
}