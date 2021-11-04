#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <signal.h>
#include <assert.h>

#include <pthread.h>



#define handle_error_en(en, msg) \
	do {errno = en; perror(msg); exit(EXIT_FAILURE); } while(0)

#define MAX_NAME_LEN 64
#define FAN "fan"	


typedef struct _user_info {
    int age;
    int sex;
    char name[MAX_NAME_LEN];
}user_info;

/* 线程处理函数*/
void  sig_thread(void *arg);
void * sub_thread_func(void * arg);


pthread_t tid, sig_tid;	    //线程id号

int main(void)
{
    int ret = -1;
    char string[] = "Hello, thread!";
    user_info *pinfo = NULL;

    printf("Main thread：threads demo!\n");
    printf("Main thread：before pthread_create()!\n");
    
    sigset_t set;
   
    sigemptyset(&set);	//清空set信号集
    
    sigfillset(&set);   //把所有信号填充到set集合

    ret = pthread_sigmask(SIG_BLOCK, &set, NULL);
    if(ret != 0)
	handle_error_en(ret, "pthread_sigmask");

    pthread_create(&tid, NULL, sub_thread_func, (void *)string);
    pthread_create(&sig_tid, NULL, (void *)sig_thread, NULL);
    
    printf("Main thread：After pthread_create()!\n");
        
    pthread_join(tid, (void *)pinfo);
    printf("After pthread_join()!\n");
#if 0
    if(pinfo){
	//...FIXME!!!
	free(pinfo);
	pinfo = NULL; 
    }
#endif
    sleep(2);

    return 0;
}

void sig_user_handler(int signo)
{
    if(SIGUSR2 == signo){
	printf("Receive a SIGUSR2 signal!\n");
    }
}

void sig_handler(int signo)
{
    if(SIGINT == signo){
	printf("Receive a SIGINT signal!\n");
	pthread_kill(tid, SIGUSR2);
    }

    if(SIGALRM == signo){
	printf("Receive a SIGALRM signal!\n");	
	exit(1);
    }
}

void sig_thread(void *arg)
{
    int i;
    int ret = -1;
    struct sigaction act;
    sigset_t set;
    
    //设置属性为 分离属性
    pthread_detach(pthread_self());
    
    act.sa_handler = sig_handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGQUIT);
    act.sa_flags = SA_RESTART;
    sigaction(SIGINT, &act, NULL);  //临时屏蔽SIGINT信号

    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGINT);
    sigaction(SIGQUIT, &act, NULL);  //	会临时屏蔽SIGQUIT信号

    act.sa_handler = SIG_DFL;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NOCLDWAIT;
	//当子进程退出时，直接释放子进程所占用的8KB物理内存
    sigaction(SIGCHLD, &act, NULL);	

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGCHLD);

    ret = pthread_sigmask(SIG_UNBLOCK, &set, NULL);
    if(ret != 0)
	handle_error_en(ret, "pthread_sigmask!");
    sigpending(&set);	//将被搁置的信号返回
    
    for(i = 0; i < 100; i++)
    {
	printf("current time is：%d ms!\n", i*500);
	usleep(500000);	  //睡眠0.5秒
    }
}

void * sub_thread_func(void * arg)
{
    char *str = (char *)arg;
    printf("child thread：arg=%s\n",str);
    user_info *uinfo;

    int ret = -1;
    sigset_t set;
    
    sigemptyset(&set);
    sigaddset(&set, SIGUSR2);	

    signal(SIGUSR2, sig_user_handler);

    ret = pthread_sigmask(SIG_UNBLOCK, &set, NULL);
    if(ret != 0)
	handle_error_en(ret, "pthread_sigmask"); 


    uinfo = (user_info *)malloc(sizeof(user_info));
    /*??????????*/
    assert(uinfo);    
    /*??????????*/
    if(!uinfo) 
	return NULL;

    uinfo->age = 20;
    uinfo->sex = 0;
    strncpy(uinfo->name, FAN, strlen(FAN));

    printf("Give user info to main thread\n"); 

    sleep(50);
    pthread_exit((void *)uinfo);

}

