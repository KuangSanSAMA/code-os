#define _GNU_SOURCE
#include "sched.h"
#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "semaphore.h"

int producer(void * args);
int consumer(void * args);
pthread_mutex_t mutex;
sem_t product;
sem_t warehouse;
//buffer is the container
char buffer[8][4];
//bp is the pointer for the buffer
int bp = 0;

void main(int argc,char **argv)
{
	//initialization
	pthread_mutex_init(&mutex,NULL);
	//int sem_init(sem_t *sem, int pshared, unsigned int value); psared:0 share in thread,other value share in process ; if success return 0 else return -1
	sem_init(&product,0,0);
	sem_init(&warehouse,0,8);
	int clone_flag,arg,retval;
	char *stack;
	
	clone_flag = CLONE_VM|CLONE_SIGHAND|CLONE_FS|CLONE_FILES;
	int i;
	for(i=0;i<2;i++)
	{
		//create thread
		arg = i;
		stack = (char *)malloc(4096);
		retval=clone((void *)producer,&(stack[4095]),clone_flag,(void *)&arg);
		stack = (char *)malloc(4096);
		retval=clone((void *)consumer,&(stack[4095]),clone_flag,(void *)&arg);
		sleep(1);
	}
	pid_t pid=0;
	//__WCLONE Wait for "clone" children only;__WALL Wait for all children, regardless of type ("clone" or "non-clone").
	for( i = 0;i<4;i++){
		pid = waitpid(-1, NULL,  __WCLONE | __WALL);
	}
	//wait for thread finish
	//sleep(60);
	exit(1);
}

int producer(void *args)
{
	int id = *((int *)args);
	int i;
	for(i=0;i<10;i++)
	{
		sleep(i+1);
		//P operation (wait for consumer release the empty space)
		sem_wait(&warehouse);
		//lock for only one thread can change buffer an bp
		pthread_mutex_lock(&mutex);
		if(id==0)
			strcpy(buffer[bp],"aaa\0");
		else
			strcpy(buffer[bp],"bbb\0");
		bp++;
		printf("producer %d produce %s in %d\n",id,buffer[bp-1],bp-1);
		//unlock
		pthread_mutex_unlock(&mutex);
		//V operation for consumer
		sem_post(&product);
	}
	printf("producer %d is over!\n",id);
}

int consumer(void *args)
{
	int id = *((int *)args);
	int i;
	for(i=0;i<10;i++)
	{
		sleep(10-i);
		//P operation (wait for producer produce product)
		sem_wait(&product);
		//lock for only one thread can change buffer an bp
		pthread_mutex_lock(&mutex);
		bp--;
		printf("consumer %d get %s in %d\n",id,buffer[bp],bp);
		strcpy(buffer[bp],"zzz\0");
		//unlock
		pthread_mutex_unlock(&mutex);
		//V operation for the producer
		sem_post(&warehouse);
	}
	printf("consumer %d is over!\n",id);
}

