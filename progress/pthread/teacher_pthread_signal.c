#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <assert.h>

#include <pthread.h>

#define MAX_NAME_LEN 64

 #define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)


typedef struct  _user_info {
    int age;
    int sex;
    char name[MAX_NAME_LEN];

}user_info;


pthread_t tid, tid_sig;

void * sub_thread_func(void *arg);
void thread_sig(void *arg);

int main(void)
{
   int ret = -1;
    user_info *pInfo = NULL;

    char string[] = "Hello, thread!";

    printf("Main thread: threads demo!\n");

    printf("Main thread:before pthread_create()!\n");

    sigset_t set;
    /* 信号处理*/
    sigemptyset(&set);
    sigfillset(&set);  //把所有的信号都加入到set集合中
    ret = pthread_sigmask(SIG_BLOCK, &set, NULL); /* 屏蔽所有信号*/
    if (ret != 0)
               handle_error_en(ret, "pthread_sigmask");

    pthread_create(&tid, NULL, sub_thread_func, (void *)string); 
    pthread_create(&tid_sig, NULL, (void *)thread_sig, NULL);
    
    printf("Main thread: After pthread_create()!\n");


    pthread_join(tid, (void *)&pInfo);
    printf("after pthread_join\n");
    sleep(2);
    return 0;
}


void sig_handler(int signo)
{
    if(SIGINT == signo) {
        printf("Receive a SIGINT signal!\n");
        pthread_kill(tid, SIGUSR2); 
    }

    if(SIGQUIT == signo) {
        printf("Receive a SIGQUIT signal!\n");
        exit(1);
    }

}

void thread_sig(void *arg)
{
  int i;
  int ret = -1;
  sigset_t set;
  struct sigaction act;

  pthread_detach(pthread_self()); //设置线程tid的属性为分离属性， 之后就"不"需要父线程去pthread_join()  
  
#if 0
    signal(SIGINT, sig_handler);
    signal(SIGQUIT, sig_handler);
#else
   act.sa_handler = sig_handler;

    sigemptyset(&act.sa_mask);
    sigaddset( &act.sa_mask, SIGQUIT);
    act.sa_flags = SA_RESTART;
    sigaction(SIGINT, &act, NULL);

    sigemptyset(&act.sa_mask);
    sigaddset( &act.sa_mask, SIGINT);

    sigaction(SIGQUIT, &act, NULL);
 #endif
  act.sa_handler= SIG_DFL;
  sigemptyset(&act.sa_mask);
  act.sa_flags =SA_NOCLDWAIT; /* 当子进程退出时，直接释放子进程所占用的8KB物理内存*/
  sigaction(SIGCHLD, &act, NULL);
  
  sigemptyset(&set);  
  sigaddset(&set, SIGINT);
  sigaddset(&set, SIGQUIT);
  sigaddset(&set, SIGCHLD);    
  
  ret = pthread_sigmask(SIG_UNBLOCK, &set, NULL); /* 放开SIGQUIT,SIGINT, SIGCHLD阻塞 */  
  if (ret != 0)
               handle_error_en(ret, "pthread_sigmask");

  sigpending(&set);
  
  for (i =0; i< 100; i++)
    {
    printf("current time is: %d ms!\n", i*500);
    usleep(500000); //睡眠0.5s
    }
    return;
}

static void sig_user_handler(int signo)
{
    if(signo == SIGUSR2) {
        printf("Receive a SIGUSR2 signal!\n");
    }
}

void sig_sub_handler(int signo)
{
    if(SIGINT == signo) {
        printf("2: Receive a SIGINT signal!\n");
    }

    if(SIGALRM == signo) {
        printf("2: Receive a SIGALRM signal!\n");
     }

}

void * sub_thread_func(void *arg)
{
    int ret = -1;
    char *p = (char *)arg;

    user_info *uInfo = NULL;
    
    sigset_t set;
    
    printf("Child thread: arg =%s\n", p);

   
    sigemptyset(&set);
    sigaddset(&set, SIGUSR2);

    signal(SIGUSR2, sig_user_handler);
//    signal(SIGINT, SIG_IGN);
//    signal(SIGINT,  sig_sub_handler);
//    signal(SIGALRM, sig_sub_handler);

    ret = pthread_sigmask(SIG_UNBLOCK, &set, NULL);
    if (ret != 0)
               handle_error_en(ret, "pthread_sigmask");

 //   sigpending(&set);
    
    uInfo = (user_info *) malloc(sizeof(user_info));
    assert(uInfo);
    if(!uInfo) return NULL;

    uInfo->age = 19;
    uInfo->sex = 0;
    strcpy(uInfo->name, "farsight");

    printf("Give the user info to main thread!\n");
    sleep(30); //睡眠15s

    pthread_exit((void *)uInfo);

}

