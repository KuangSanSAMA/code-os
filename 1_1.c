#include "sys/types.h"
#include "sys/file.h"
#include "unistd.h"

char r_buf[4];
char w_buf[4];
int pipe_fd[2];
pid_t pid1,pid2,pid3,pid4;

int producer(int id);
int consumer(int id);

int main(int argc,char **argv)
{	
	//create pipe file, 0 is success ,-1 is fail
	if(pipe(pipe_fd)<0)
	{
		printf("pipe create error \n");
		exit(-1);
	}
	else
	{	
		//father create son, sons do their function
		printf("pipe is created successfully! \n");
		if((pid1=fork())==0)
			producer(1);
		if((pid2=fork())==0)
			producer(2);
		if((pid3=fork())==0)
			consumer(1);
		if((pid4=fork())==0)
			consumer(2);
	}
	//only shut down the writer can run correctly
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	//father wait son finish their function
	//as long as a son process dead , then return the dead son process pid
	int i,pid,status;
	for(i=0;i<4;i++)
		pid = wait(&status);
	exit(0);
}
int producer(int id)
{
	printf("producer %d is running! \n",id);
	close(pipe_fd[0]);
	int i=0;
	for(i =1;i<10;i++)
	{
		sleep(3);
		if(id==1)
			strcpy(w_buf,"aaa\0");
		else
			strcpy(w_buf,"bbb\0");
		//write and test
		//if there is no reader , write operation will fail
		if(write(pipe_fd[1],w_buf,4)==-1)
			printf("write to pipe error\n");
	}
	close(pipe_fd[1]);
	printf("producer %d is over! \n",id);
	exit(id);
}

int consumer(int id)
{
	close(pipe_fd[1]);
	printf("consumer %d is running! \n",id);
	if(id==1)
		strcpy(w_buf,"ccc\0");
	else
		strcpy(w_buf,"ddd\0");
	while(1)
	{
		sleep(1);
		strcpy(r_buf,"eee\0");
		//only writers all be closed and read finish then reutrn 0
		//there is writer and read finish ,reader will wait 
		if(read(pipe_fd[0],r_buf,4)==0)
			break;
		printf("consumer %d get %s ,while the w_buf is %s\n",id,r_buf,w_buf);
	}
	close(pipe_fd[0]);
	printf("consumer %d in over! \n",id);
	exit(id);
}	
