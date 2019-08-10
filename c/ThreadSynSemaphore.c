/*
 * Thread Synchronization - Mutex
 *
 * int sem_init(sem_t *sem, int pshared, unsigned int value)
 * int sem_wait(sem_t *sem)
 * int sem_post(sem_t *sem)
 * int sem_destroy(sem_t *sem)
 *
 * Copyright (c) 2016 Liming Shao <lmshao@163.com>
 */
 
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define SIZE 4
#define OVER (-1)

struct producer
{
    int buffer[SIZE];
    int rpos, wpos;
    sem_t sem_read;
    sem_t sem_write;
};

struct producer buffer;

void Init(struct producer *b)
{
    sem_init(&b->sem_write, 0, SIZE-1);
    sem_init(&b->sem_read, 0, 0);
    b->rpos = 0;
    b->wpos = 0;
}

void Put(struct producer *b, int data)
{
    sem_wait(&b->sem_write);
    b->buffer[b->wpos++] = data;
    
    if(b->wpos >= SIZE)
        b->wpos = 0;
    
    sem_post(&b->sem_read);
}

int Get(struct producer *b)
{
    int data;
    sem_wait(&b->sem_read);
    
    data = b->buffer[b->rpos++];
    if(b->rpos >= SIZE)
        b->rpos = 0;
    
    sem_post(&b->sem_write);
    
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
