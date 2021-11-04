#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#define QUIT_STR "quit"


int main(void)
{
    int pipefd[2];
    pid_t pid = -1;

    printf("uname pipe demo!\n");

    if(pipe(pipefd) < 0){   //创建无名管道
	perror("pipe failed!");
	exit(1);
    }

    if((pid = fork()) < 0){  //	创建子进程 
	perror("pid fork failed!");
	exit(1);
    }
    /* 子进程会继承父进程打开的管道，实现子进程写，父进程读 */

    //pipefd[0] 读端，pipefd[1] 写端
    
    if(pid == 0){   //子进程内
	close(pipefd[0]);
	while(1)
	{
	    char buf[BUFSIZ];
	    fprintf(stdout,"请输入字符串:");
	    bzero(buf ,BUFSIZ);

	    if(fgets(buf ,BUFSIZ-1 ,stdin) == NULL){
		perror("fgets failed!");
		continue;
	    }

	    write(pipefd[1] ,buf ,strlen(buf));

	    if(!strncasecmp(buf ,QUIT_STR ,strlen(QUIT_STR)))  //用户输入quit
		break;
	}
	close(pipefd[1]);
	exit(0);
    }

    if(pid > 0){   //父进程中
	close(pipefd[1]);
	char buf[BUFSIZ];

	while(1)
	{
	    bzero(buf ,BUFSIZ);
	    read(pipefd[0] ,buf ,sizeof(buf));
	    //数据处理
	    //...FIXME!!!
  
	    printf("buf have：%s",buf);
	    if(!strncasecmp(buf ,QUIT_STR ,strlen(QUIT_STR)))  //用户输入quit
		break;
	}
	close(pipefd[0]);
	exit(0);
	    
    }
    return 0;
}
