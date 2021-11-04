#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <pthread.h>

#define MAX_NAME 30
#define ZHANGSAN "张三"

typedef struct persanal_info{
    int age;
    int sex;
    char name[MAX_NAME];
}per_info, *per_pinfo;

void *sub_thread_routine(void  *agr)	
{
    //...FIXME!!!
        
    per_pinfo p; 
    p = (per_pinfo)malloc(sizeof(per_info));
    if(!p){
	perror("malloc failed!");
	return NULL; 
    }
    
    p->age = 18;
    p->sex = 1;
    strncpy(p->name, ZHANGSAN, strlen(ZHANGSAN));
    
    char *str = (char *)agr;
    printf("I am sub thread： agr = %s\n",str);

    sleep(2);

    /* 2.退出线程*/
    pthread_exit((void *)p);
}

int main(void)
{
    pthread_t tid;
    char *string = "hi fanfan!";
    per_pinfo pinfo = NULL;

    printf("Multithread demo!\n");

    /* 1.创建线程*/		    //第四个参数为第三个参数(函数)的行参
    pthread_create(&tid, NULL, (void *)sub_thread_routine, (void *)string);
    
    printf("Before pthread_join()\n");

    pthread_join(tid, (void *)&pinfo);
    if(pinfo){
	printf("age=%d,sex=%d,name=%s\n",pinfo->age, pinfo->sex, pinfo->name);
	free(pinfo);
	pinfo = NULL;
    }

    printf("********After pthread_join()\n");

    return 0;
}


