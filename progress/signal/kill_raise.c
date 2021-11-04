#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <signal.h>
#include <sys/wait.h>


int main(void)
{
    pid_t pid = -1;

    printf("kill() and raise() deom!\n");

    if((pid = fork()) < 0){
	perror("fork failed!");
	exit(1);
    }

    if(pid == 0){   //子进程中
	printf("child say: I'll stop work!\n");

//	kill(pid, SIGQUIT);
	raise(SIGQUIT);

	printf("I'am child progress my pid = %d\n",pid);
	exit(0);
    }

    if(pid > 0){    //父进程中
	usleep(200000);   //睡眠0.2秒等待子进程的信号
	if(waitpid(pid, NULL, WNOHANG) == 0){	//查询子进程推出状态，没有退出	    
	    printf("parent say: kill child progress(pid = %d)\n",pid);
	    kill(pid, SIGKILL);
	}
    }

    return 0;
}
