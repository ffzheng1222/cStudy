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

    sleep(2);

    /* 2.退出线程*/
    pthread_exit(NULL);
}

int main(void)
{
    pthread_t tid;
    char *string = "hi fanfan!";

    printf("Multithread demo!\n");

    /* 1.创建线程*/		    //第四个参数为第三个参数(函数)的行参
    pthread_create(&tid, NULL, (void *)sub_thread_routine, (void *)string);
    
    printf("Before pthread_join()\n");

    pthread_join(tid, NULL);

    printf("After pthread_join()\n");

    return 0;
}


