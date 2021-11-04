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
    int ret,write_fd = -1;

    if(argc < 2){
	//参数处理
	usage(argv[0]);
	exit(1);
    }

    /* 判断管道文件是否存在 */
    if(access(argv[1],F_OK) != 0)
    {	//文件不存在  创建:
	umask(0);  //放开文件权限

	//创建有名管道文件
	if(mkfifo(argv[1],0666) < 0){
	    perror("mkfifo failed!");
	    exit(1);
	}
    }
    else
    {  // 文件存在  获得文件属性:
	struct stat st;
	if(stat(argv[1],&st) < 0){
	    perror("stat failed!");
	    usage(argv[0]);
	    exit(1);
	}

	if((st.st_mode & S_IFMT) != S_IFIFO)
	{	 //如果不是管道文件

	    unlink(argv[1]);	//删除文件
	    umask(0);  //放开文件权限

	    //创建有名管道文件
	    if(mkfifo(argv[1],0666) < 0){
		perror("mkfifo failed!");
		exit(1);
	    }   
	}
    }
    write_fd = open(argv[1],O_RDWR);

    if(write_fd <0){
	perror("open failed!");
	exit(1);
    }

    while(1)
    {
	char buf[BUFSIZ];
	bzero(buf ,BUFSIZ);

	/* 从标准键盘输入 */
	fprintf(stderr,"请输入字符串:");
	if(fgets(buf ,BUFSIZ-1 ,stdin) == NULL){
	    perror("fgets failed!");
	    continue;
	}
#if 0
	if(ret < 0){
	    perror("write failed!");
	    exit(1);
	}
#else
	do{   //阻塞函数的工程写法
	    ret = write(write_fd ,buf ,BUFSIZ-1);
	}while(ret < 0 && EINTR == errno);
#endif

	if(!strncasecmp(buf ,QUIT_STR ,strlen(QUIT_STR)))  //用户输入quit退出
	    break;
    }
    close(write_fd);

    return 0;
}

