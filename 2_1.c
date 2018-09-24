#include "math.h"
#include "sched.h"
#include "pthread.h"
#include "stdlib.h"
#include "semaphore.h"
#include "stdio.h"

typedef struct{
    char task_id;
    int call_num;
    int ci;//cpu time
    int ti;//turnaround time
    int ci_left;
    int ti_left;
    int flag;//if the proc is alive
    int arg;
    pthread_t th;
} task;

void proc(int * args);
void * idle();
int select_proc();

int task_num= 0;
int idle_num =0;

int alg;
int curr_proc = -1;
int demo_time = 100;

task * tasks;
pthread_mutex_t proc_wait[100];
pthread_mutex_t main_wait,idle_wait;
float sum =0;
pthread_t idle_proc;

int main(int argc,char **argv){
	//main lock and idle lock init 
    pthread_mutex_init(&main_wait,NULL);
    pthread_mutex_lock(&main_wait);
    pthread_mutex_init(&idle_wait,NULL);
    pthread_mutex_lock(&idle_wait);
    printf("Please input number of real time tasks:\n");
    scanf("%d",&task_num);
    tasks = (task *)malloc(task_num * sizeof(task));
    int i;
    for(i =0;i<task_num;i++){
		//proc lock init
        pthread_mutex_init(&proc_wait[i],NULL);
        pthread_mutex_lock(&proc_wait[i]);
    }
    for(i = 0;i<task_num;i++){
    	//get every lock message
        printf("Please input task id ,followed by Ci and Ti :\n");
        getchar();
        scanf("%c,%d,%d",&tasks[i].task_id,&tasks[i].ci,&tasks[i].ti);
        printf("id:%c,ci:%d,ti:%d\n",tasks[i].task_id,tasks[i].ci,tasks[i].ti);
        tasks[i].ci_left = tasks[i].ci;
        tasks[i].ti_left = tasks[i].ti;
        tasks[i].flag = 2;
        tasks[i].arg = i;
        tasks[i].call_num = 1;
        //compute the feasibility
        sum = sum+(float)tasks[i].ci/(float)tasks[i].ti;
    }
    printf("Please input algorithm,1 for EOF, 2 for RMS:");
    //getchar();
    scanf("%d",&alg);
    printf("Please input demo time:");
    scanf("%d",&demo_time);
    double r = 1;
    if(alg == 2){
        r = ((double)task_num) * (exp(log(2)/(double)task_num)-1);
        printf("r is %lf\n",r);
        
    }
    if(sum > r){
        printf("(sum = %lf>r = %lf),not schedulable!\n",sum,r);
        exit(2);
    }
    //int pthread_create(pthread_t *restrict tidp,const pthread_attr_t *restrict_attr,void*ги*start_rtn)(void*),void *restrict arg);
    pthread_create(&idle_proc,NULL,(void *)idle,NULL);
    //create proc thread
    for(i = 0;i<task_num;i++)
        pthread_create(&tasks[i].th,NULL,(void *)proc,&tasks[i].arg);
    //every demo_time will return main and select(may return curr_id and continue current thread)
    for(i = 0;i<demo_time;i++){
        int j;
        if((curr_proc = select_proc(alg))!= -1){
            pthread_mutex_unlock(&proc_wait[curr_proc]);
            pthread_mutex_lock(&main_wait);
        }
        else{
            pthread_mutex_unlock(&idle_wait);
            pthread_mutex_lock(&main_wait);
        }
        for(j = 0;j<task_num;j++){
        	//compute time for ti ; if ti == 0 then new task come and set it
            if(--tasks[j].ti_left==0){
                tasks[j].ti_left = tasks[j].ti;
                tasks[j].ci_left = tasks[j].ci;
                pthread_create(&tasks[j].th,NULL,(void *) proc,&tasks[j].arg);
                tasks[j].flag = 2;
            }
        }
    }
    printf("\n");
    sleep(10);
}

void proc(int *args){
    while(tasks[* args].ci_left>0){
        pthread_mutex_lock(&proc_wait[* args]);
        //if pre thread is idle then display the time idle run
        if(idle_num!=0){
            printf("idle(%d)",idle_num);
            idle_num =0;
        }
        printf("%c%d",tasks[*args].task_id,tasks[*args].call_num);
        tasks[*args].ci_left--;
        //judge if current thread is alive
        if(tasks[*args].ci_left==0){
            printf("(%d)",tasks[*args].ci);
            tasks[*args].flag= 0;
            tasks[*args].call_num++;
        }
        pthread_mutex_unlock(&main_wait);
    }
}

void * idle(){
    while(1){
        pthread_mutex_lock(&idle_wait);
        printf("->");
        idle_num++;
        pthread_mutex_unlock(&main_wait);
    }
}

int select_proc(int alg){
    int j ;
    int temp1,temp2;
    temp1 = 10000;
    temp2 = -1;
    //in RMS if curr not finish then not change the thread
    if((alg ==2)&&(curr_proc!= -1)&&(tasks[curr_proc].flag != 0)){
        return curr_proc;
    }
    for(j = 0;j<task_num;j++){
        if(tasks[j].flag ==2){
            switch(alg){
                case 1:
                	//find the least ti_left 
                    if(temp1>tasks[j].ti_left){
                        temp1 = tasks[j].ti_left;
                        temp2 = j;
                    }
                    break;
                case 2:
                	//find the least ti
                    if(temp1>tasks[j].ti){
                        temp1 = tasks[j].ti;
                        temp2=j;
                    }
                    break;
            }
        }
    }
    return temp2;
}
