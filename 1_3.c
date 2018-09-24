#define _GNU_SOURCE
#include <stdlib.h>   
#include <sys/shm.h>
#include <sys/ipc.h>  
#include <unistd.h>  
#include <string.h>

#include "sched.h"
#include "pthread.h"
#include "stdio.h"
#include "semaphore.h"

sem_t product;
sem_t warehouse;

#define SIZE 4

int producer(void * args);
int consumer(void * args);
char * shmAddr;
int main(){
    sem_init(&product,0,0);
	sem_init(&warehouse,0,1);
    int shmID = shmget((key_t)1234,SIZE,0666|IPC_CREAT);

    if (shmID ==-1){
        printf("create shmID error");
        exit(0);
    }
    shmAddr = shmat(shmID,NULL,0);
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
    //printf("1");
	pid_t pid=0;
	//__WCLONE Wait for "clone" children only;__WALL Wait for all children, regardless of type ("clone" or "non-clone").
	for( i = 0;i<4;i++){
		pid = waitpid(-1, NULL,  __WCLONE | __WALL);
	}
    shmdt(shmAddr);
    shmctl(shmID, IPC_RMID, NULL);
	exit(0);
}

int producer(void * args){
    int id = *((int *)args);
    char * dataAddr;
    if (id == 1)
        dataAddr = "aaa\0";
    else
        dataAddr = "bbb\0";
    //printf("222");
    
    int i;
    for(i= 0;i<5;i++){
        sleep(i+1);
        sem_wait(&warehouse);
        strcpy(shmAddr,dataAddr);
        printf("producer %d produce %s\n",id,dataAddr);
        sem_post(&product);
    }
    //shmdt(shmAddr);
    //exit(1);
}

int consumer(void * args){
    int id = *((int *)args);
    //printf("111");
	int i;
    for(i=0;i<5;i++){
        sleep(5-i);
        sem_wait(&product);
        
        printf("consumer %d get %s\n",id,shmAddr);
        sem_post(&warehouse);
    }
    //shmdt(shmAddr);
    //exit(2);
}
