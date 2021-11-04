#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <pthread.h>

void *sub_thread_routine(void  *agr)	
{
    //...FIXME!!!
    
    char *str = (char *)agr;
    printf("I am sub thread： agr = %s\n",str);

//    /* 3.把线程设置为分离属性*/
//    pthread_detach(tid);    

    sleep(2);
    printf("Sub:After sleep!\n");

    /* 2.线程退出函数*/
    pthread_exit(NULL);
}

int main(void)
{
    pthread_t tid;
    char *string = "hi fanfan!";

    printf("Multithread demo!\n");

    /* 1.创建线程*/		    //第四个参数为第三个参数(函数)的行参
    pthread_create(&tid, NULL, (void *)sub_thread_routine, (void *)string);
    printf("Main:After pthread_create()\n");
 
    /* 3.把线程设置为分离属性*/
    pthread_detach(tid);    
    printf("Main:After pthread_detach()\n");

    sleep(3);
    printf("Main:After sleep!\n");

    return 0;
}


