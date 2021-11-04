#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include <assert.h>

#include <pthread.h>

#define MAX_NAME_LEN 64
typedef struct  _user_info {
    int age;
    int sex;
    char name[MAX_NAME_LEN];

}user_info;


void * sub_thread_func(void *arg)
{
    char *p = (char *)arg;

    user_info *uInfo = NULL; 
    printf("Child thread: arg =%s\n", p);
    
//    pthread_detach(pthread_self()); //设置线程tid的属性为分离属性， 之后就"不"需要父线程去pthread_join()

    uInfo = (user_info *) malloc(sizeof(user_info));
    assert(uInfo);
    if(!uInfo) return NULL;

    uInfo->age = 19;
    uInfo->sex = 0;
    strcpy(uInfo->name, "farsight");    


    printf("Child thread: before pthread_exit()\n");
    pthread_exit((void *)uInfo);
    //return NULL;
}


int main(void)
{

    pthread_t tid;
    
    //user_info *pInfo = NULL;

    pthread_attr_t attr;

    char string[] = "Hello, thread!";

    printf("Main thread: threads demo!\n");
    
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED); //设置分离属性
    
    /*把默认的栈的大小放大一倍 */
    size_t stacksize;
    pthread_attr_getstacksize(&attr, &stacksize);
    printf("Thread stack size: %dMB\n", stacksize/1024/1024);
    pthread_attr_setstacksize(&attr, stacksize*2);
    
    pthread_create(&tid, &attr, sub_thread_func, (void *)string); 

    pthread_attr_destroy(&attr);
 
   //pthread_detach(tid); //设置线程tid的属性为分离属性， 之后就"不"需要父线程去pthread_join()
     printf("Main thread: After pthread_create()!\n");


//    pthread_join(tid, (void *)&pInfo);
      printf("Main thread: Child is detach thread!\n"); //父线程不需要去pthread_join()等待，同时也不能取得子进程的返回值 
      sleep(2);

      return 0;
}
