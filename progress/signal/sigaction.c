#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

//int sig_a = 1;

void sig_ctrl_handler(int sig_on)
{
    if(sig_on == SIGINT){
	printf("Receive a SIGINT signal!\n");
	printf("暂不处理 \tctrl+c\t 信号!\n");
//	sig_a = 0; 
    }
    if(sig_on == SIGQUIT){
	printf("\nReceive a SIGQUIT signal!\n");
	printf("处理 \tctrl+\\\t 信号!\n");
	exit(0);
    }
}

int main(void)
{
    printf("signal deam:\n");

#if 0
    /********* crtl+c 信号*********/
    signal(SIGINT, sig_ctrl_handler);	

    /********* crtl+\ 信号*********/
    signal(SIGQUIT, sig_ctrl_handler);

#else
//    sigset_t set ;
    struct sigaction act;

    act.sa_handler = sig_ctrl_handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask,SIGQUIT);
    act.sa_flags = 0;

    if(sigaction(SIGINT, &act, NULL) < 0) {  //回临时屏蔽SIGINT信号	
	perror("sigaction failed!");
	exit(1);
    }

    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask,SIGINT);

    if(sigaction(SIGQUIT, &act, NULL) < 0) {  //回临时屏蔽SIGQUIT信号
	perror("sigaction failed!");
	exit(1);
    }


    /* 把父子进程进行分离 ，分离之后 ，父进程就不需要也不能调用wait()/waitpid()*/
    /******项目工程上的用法*******/
    act.sa_handler = SIG_DFL;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_NOCLDWAIT;
    sigaction(SIGCHLD, &act, NULL);
#endif

#if 1
    //main主流程
    while(1)
    {
	printf("你是个二货\n");
	usleep(800000);	//睡眠0.2秒
    }
#else
    while(sig_a)
    {
	printf("你是个二货\n");
	usleep(200000)	//睡眠0.2秒
    }
#endif

    return 0;
}
