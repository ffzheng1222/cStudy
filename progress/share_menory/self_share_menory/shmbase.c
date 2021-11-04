#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>


#define SHM_KEY_PATH "/"
#define SHM_KEY_PROJ_ID 'a'
#define SHM_SIZE 2048


/* 程序中 #if/#endif 可不需要*/
int main(void)
{
    int shmid = -1;
    char *pshm = NULL;

    /* 键值key的获取方法*/
#if 0
    //1.直接指定键值
    shmid = shmget("0x12345678", ...);
    //3.从系统申请私有键值
    shmid = shmget(IPC_PRIVATE, ...);
#else

    //2.通过fock函数获取键值
    key_t key;	 //接收键值的变量key
    if( (key = ftok(SHM_KEY_PATH, SHM_KEY_PROJ_ID)) < 0){
	perror("ftok failed!");
	exit(1);
    }
#endif

    /*1.创建或关联共享内存*/
    shmid =  shmget(key, SHM_SIZE, IPC_CREAT|0666);
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
#if 1
    printf("after shmat()!\t");
    printf("映射共享内存后的效果!\n");
    system("ipcs -m");	//方便观察效果,可不写入程序
#endif

    /*3.使用共享内存*/
	 ///...FIXME

    /*4.撤销用户空间的映射*/
    if(shmdt((void *)pshm) < 0){
	perror("shmdt failed!");
	exit(1);
    }
#if 1
    printf("after shmdt()!\t");
    printf("在用户层撤销共享内存后的效果!\n");
    system("ipcs -m");	//方便观察效果,可不写入程序
#endif

    /*5.删除内核中创建的共享内存*/
    if(shmctl(shmid, IPC_RMID, NULL) < 0){
	perror("shmctl failed!");
	exit(1);
    }
#if 1
    printf("after shmctl()!\t");
    printf("在内核层删除共享内存后的效果!\n");
    system("ipcs -m");	//方便观察效果,可不写入程序
#endif

    return  0;
}
