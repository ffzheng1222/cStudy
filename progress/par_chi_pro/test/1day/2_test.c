#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

int fork_pid(pid_t pid,int n)
{
      if(0 == n)
	    exit(1);

      else{
	    pid = fork();

	    if(pid < 0){
		  perror("pid fork failed!");
		  exit(1);
	    }
	    else if(pid > 0){  //父进程
		  printf("\n我是子进程: my pid = %d\t我的父进程：my ppid = %d\n",getpid(),getppid());
		  sleep(1);
		  exit(0);
	    }
	    else{  //子进程
		  n--;
		  fork_pid(getpid(),n);
	    }
      }
      return 0;
}

int main(void)
{
      pid_t  ret = -1;
      int n;

      printf("请您输入需创建的子进程数:");
      scanf("%d",&n);

      if((ret = fork()) < 0){
	    perror("fork failed!");
	    exit(1);
      }

      if(ret > 0){
	    printf("我是父进程： my pid = %d\n ",getpid());
	    sleep(2);
	    exit(0);
      }
      if(ret == 0){   //子进程
	    fork_pid(getpid(),n);
      }

      return 0;
}
