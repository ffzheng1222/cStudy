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
		printf("I'am chiled progress! \
		my pid = %d\tmy ppid = %d\n", getpid(), getppid());
		exit(0);
	    default:	//父进程
		waitpid(-1 ,NULL ,0);
		break;
	}
    }
    printf("\nI'am parent progress!   \tmy pid = %d\n",getpid());
    return 0;
}
