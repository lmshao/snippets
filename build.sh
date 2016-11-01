#!/bin/sh
gcc -o ThreadSynCondition ThreadSynCondition.c -lpthread
gcc -o ThreadSynMutex ThreadSynMutex.c -lpthread
gcc -o ThreadSynSemaphore ThreadSynSemaphore.c -lpthread