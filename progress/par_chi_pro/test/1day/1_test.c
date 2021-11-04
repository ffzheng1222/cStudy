#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

int main(void)
{
      pid_t pid = -1;
      int i,n;

      printf("请输入需创建的子进程个数:");
      scanf("%d",&n);

      for(i = 0; i < n; i++)
      {
	    pid = fork();

	    if(pid < 0 || pid == 0)
	    {
		  break;
	    }
      }

      if(pid < 0)
      {
	    perror("fork failed!");
	    exit(1);
      }

      else if(pid == 0)   //子进程
      { 
	    printf("\n子进程 my pid:%d\t其对应的父进程ppid:%d：\n",getpid(),getppid());
	    exit(0);
      }

      else  
      {	    //父进程
	    printf("\n父进程 my pid:%d\n",getpid());

	    usleep(200000);

	    exit(0);
      }
      
      return 0;      
}
