#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sys/file.h"
#include "unistd.h"

int producer(int id);
int consumer(int id);
pid_t pid1,pid2;
int msg_id;

struct message{
    char mess[4];
}writer,reader;
int main(){
    if(msg_id = msgget(123, IPC_CREAT | 0666) == -1){
        printf("create msg error\n");
        exit(0);
    }
    if((pid1=fork())==0)
		producer(1);
    if((pid2=fork())==0)
		consumer(1);
    int i,pid,status;
	for(i=0;i<2;i++)
		pid = wait(&status);
    //msgctl(msg_id, IPC_RMID, 0);
	exit(0);
}

int producer(int id){
    msg_id = msgget(123,0666);
    strcpy(writer.mess,"aaa\0");
    if(msgsnd(msg_id,&writer,sizeof(writer),0)==-1){
        printf("write error\n");
        exit(id);
    }
    //printf("%d\n",sizeof());
    printf("producer %d is over\n",id);
    exit(id);
}
int consumer(int id){
    sleep(1);
    int msg_id = msgget(123, 0);
    strcpy(reader.mess,"bbb\0");
    if(msgrcv(msg_id,&reader,sizeof(reader),0,IPC_NOWAIT)==-1){
        printf("read error\n");
        exit(id);
    }
    printf("%s\n",reader.mess);
    printf("consumer %d is over\n",id);
    exit(id);
}