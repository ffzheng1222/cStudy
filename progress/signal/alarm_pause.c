#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>


void sig_ctrl_handler(int sig_on)
{
    if(sig_on == SIGALRM){
	printf("Receive a SIGALRM signal!\n");
    }

    if(sig_on == SIGQUIT){
	printf("Receive a SIGQUIT signal!\n");
	printf("处理 \tctrl+反斜杠\t 信号!\n");
	exit(0);
    }
}

int main(void)
{
    printf("signal deam:\n");

    /********* crtl+c 信号*********/
    signal(SIGALRM, sig_ctrl_handler);	//1.捕获SIGLARM信号

#if 0
    signal(SIGINT，SIG_IGN);	//2.忽视信号
    signal(SIGINT, SIG_DFL);	//3.系统默认处理
#endif

    /********* crtl+\ 信号*********/
    signal(SIGQUIT, sig_ctrl_handler);

#if 0
    signal(SIGINT，SIG_IGN);	
    signal(SIGINT, SIG_DFL);	
#endif

#if 1
    alarm(3);	//定义3秒的闹钟
    printf("after alarm()!\n");

    pause();	//暂停程序

    printf("\nafter pause()!\n");
#else
    while(sig_a)
    {
	printf("你是个二货\n");
	usleep(200000)	//睡眠0.2秒
    }
#endif

    return 0;
}

