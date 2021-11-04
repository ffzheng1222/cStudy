#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>


#define SHM_KEY_PATH "."
#define SHM_KEY_PROJ_ID 'a'
#define SHM_SIZE 2048
#define QUIT_STR "quit"


int main(void)
{
    int shmid = -1;
    char *pshm = NULL;
    key_t key;	 //接收键值的变量key

    if( (key = ftok(SHM_KEY_PATH, SHM_KEY_PROJ_ID)) < 0){
	perror("ftok failed!");
	exit(1);
    }

    /*1.创建或关联共享内存*/
    shmid = shmget(key, SHM_SIZE, IPC_CREAT|0666);
    //若共享内存已经存在就关联，不存在就衣0666的权限创建共享内存
    if(shmid < 0){
	perror("shmget failed!");
	exit(1);
    }

    /*2.把内核中的共享区域映射到用空间*/
    pshm = (char *)shmat(shmid, NULL, 0);
    if(pshm == (void *)-1){
	perror("shmat failed!");
	exit(1);
    }

    /*3.使用共享内存*/
    while(1)
    {
	if(strlen(pshm))
	{
	    printf("reader said：%s\n",pshm);
	    if(!strncasecmp(pshm, QUIT_STR, strlen(QUIT_STR))){
		break;
	    }
	}
	bzero(pshm, SHM_SIZE);  //清空共享内存
    }
    ///...FIXME!!!

    /*4.撤销用户空间的映射*/      
	//操作完双方都需要撤销用户空间的映射
    if(shmdt((void *)pshm) < 0){
	perror("shmdt failed!");
	exit(1);
    }

    /*5.删除内核中创建的共享内存*/    
	    //数据接收方操作完后，(一般)会删除真实创建的共享内存
    if(shmctl(shmid, IPC_RMID, NULL) < 0){
	perror("shmctl failed!");
	exit(1);
    }

    return  0;
}
