#include <stdio.h>                                                                       
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
	  
#define QUIT_STR "quit"

void usage(char *s)
{
    printf("usage: %s fifo_file\n",s);
}


int main(int argc,char *argv[])
{
    int ret,read_fd = -1;

    if(argc < 2){
	//参数处理
	usage(argv[0]);
	exit(1);
    }
#if 1
    umask(0);  //放开文件权限
    //创建有名管道文件
    if(mkfifo(argv[1],0666) < 0){
	perror("mkfifo failed!");
	exit(1);
    }
#endif

    char buf[BUFSIZ];
    read_fd = open(argv[1],O_RDWR);

    if(read_fd <0){
	perror("open failed!");
	exit(1);
    }

    while(1)
    {
	bzero(buf ,BUFSIZ);
#if 0
	ret = read(read_fd ,buf ,BUFSIZ-1);
	if(reit < 0){
	    perror("read failed!");
	    exit(1);
	}
#else
	do{   //阻塞函数的工程写法
	    ret = read(read_fd ,buf ,BUFSIZ-1);
	}while(ret < 0 && EINTR == errno);
#endif
	printf("read_fifo:%s\n",buf);

	if(!strncasecmp(buf ,QUIT_STR ,strlen(QUIT_STR)))  //用户输入quit退出
	    break;
    }
    close(read_fd);

    return 0;
}

