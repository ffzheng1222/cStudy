#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MATH 10

int main(void)
{
    pid_t pid;
    int i;

    for(i = 0 ;i < MATH ;i++)
    {
	pid = fork();

	switch(pid)
	{
	    case -1:	//出错处理
		perror("fork failed!");
		exit(1);

	    case  0:	//子进程
		printf("我是子进程 pid = %d\t我的父进程是 pid = %d\n",getpid(),getppid());	
		break;

	    default:	//父进程
		waitpid(-1 ,NULL ,0);
		printf("pid = %d\n",pid);
		exit(0);
	}
    }
    return 0;
}
