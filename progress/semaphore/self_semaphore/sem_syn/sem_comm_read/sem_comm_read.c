#include "sem_comm.h"

int main(void)
{
    int shmid = -1, semid = -1;
    char *pshm = NULL;


    /*1.创建或关联共享内存*/
    shmid = shmget(ftok(SHM_KEY_PATH, SHM_KEY_PROJ_ID), SHM_SIZE, IPC_CREAT|0666);
    //若共享内存已经存在就关联，不存在就以0666的权限创建共享内存
    semid = semget(ftok(SEM_KEY_PATH, SEM_KEY_PROJ_ID), 2, IPC_CREAT|0666);
    //若信号量已经存在就关联，不存在就以0666的权限创建信号量
    if(shmid < 0 ||semid < 0){
	perror("shmget/semget failed!");
	exit(1);
    }

    /*2.把内核中的共享区域映射到用空间*/
    pshm = (char *)shmat(shmid, NULL, 0);
    if(pshm == (void *)-1){
	perror("shmat failed!");
	exit(1);
    }
    
    //init_sem(semid, 0, 1);   //写端已经初始化信号量,故此处不需要初始化

    /*3.使用共享内存*/
    while(1)
    {
	sem_p(semid, SEM_R, 1);	//Psem_r操作

//	if(strlen(pshm))
//	{
	    printf("reader said：%s\n",pshm);

	    if(!strncasecmp(pshm, QUIT_STR, strlen(QUIT_STR))){
		break;
	    }
	    sem_v(semid, SEM_W, 1);	    //Vsem_w操作
//	}
	bzero(pshm, SHM_SIZE);  //清空共享内存
    }
    ///...FIXME!!!

    sem_v(semid, SEM_W, 1);	    //Vsem_w操作

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
    
    del_sem(semid);	//删除信号量
    
    return  0;
}
