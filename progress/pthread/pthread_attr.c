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

    /* 3.线程退出函数*/
    pthread_exit(NULL);
}

int main(void)
{
    pthread_t tid;
    char *string = "hi fanfan!";

    printf("Multithread demo!\n");

    /* 1.改变线程的属性*/
    pthread_attr_t attr;
    pthread_attr_init(&attr);	//1.初始化属性变量
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);    //2.设置分离属性
    size_t stacksize;
    pthread_attr_getstacksize(&attr, &stacksize);    //获取创建的线程栈大小
    printf("The default stacksize: %dMB\n", stacksize/1024/1024);
    pthread_attr_setstacksize(&attr, stacksize*2);	//3.改变线程栈大小(扩大2倍)
    printf("The After change stacksize: %dMB\n", stacksize*2/1024/1024);


    /* 2.线程创建函数*/		       //第四个参数为第三个参数(函数)的行参
    pthread_create(&tid, &attr, (void *)sub_thread_routine, (void *)string);

    pthread_attr_destroy(&attr);    //4.删除属性变量
    
    sleep(3);

    return 0;
}


