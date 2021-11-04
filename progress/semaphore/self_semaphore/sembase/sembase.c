#include "sem_comm.h"


/* 程序中 部分#if/#endif 可不需要*/
int main(void)
{
    int shmid = -1;
    int semid = -1; 
    char *pshm = NULL;

    /*(共享内存/信号量)键值key的获取方法*/
#if 0
    // 1.直接指定键值
    shmid = shmget("0x12345678", ...);
    semid = semget("0x12345678", ...);
    // 3.从系统申请私有键值
    shmid = shmget(IPC_PRIVATE, ...);
    semid = semget(IPC_PRIVATE, ...);
#endif

    //2.通过fock函数获取键
#if 0
    key_t shm_key;	 //接收共享内存键值的变量shm_key
    key_t sem_key;	 //接收信号量键值的变量sem_key
    shm_key = ftok(SHM_KEY_PATH, SHM_KEY_PROJ_ID);
    sem_key = ftok(SEM_KEY_PATH, SEM_KEY_PROJ_ID);
    if( shm_key < 0 || sem_key < 0){
	perror("shmget/semget failed!");
	exit(1);
    }

    /*1.创建或关联共享内存*/
    shmid = shmget(shm_key, SHM_SIZE, IPC_CREAT|0666);
	//若共享内存已经存在就关联，不存在就衣0666的权限创建共享内存
    semid = semget(sem_key, 1,	IPC_CREAT|0666);
	//若信号量已经存在就关联，不存在就衣0666的权限创建信号量
    if(shmid < 0 || semid < 0){
	perror("shmget failed!");
	exit(1);
    }
#else

    /*1.创建或关联共享内存*/
    shmid = shmget(ftok(SHM_KEY_PATH, SHM_KEY_PROJ_ID), SHM_SIZE, IPC_CREAT|0666);
	//若共享内存已经存在就关联，不存在就衣0666的权限创建共享内存
    semid = semget(ftok(SEM_KEY_PATH, SEM_KEY_PROJ_ID), 1, IPC_CREAT|0666);
	//若信号量已经存在就关联，不存在就衣0666的权限创建信号量
    if(shmid < 0 || semid < 0){
	perror("shmget failed!");
	exit(1);
    }

#endif

    /*2.把内核中的共享区域映射到用空间*/
    pshm = (char *)shmat(shmid, NULL, 0);
    if(pshm == (void *)-1){
	perror("shmat failed!");
	exit(1);
    }

	// 3.0使用信号量之前必须 初始化信号量
	    //参数1:semget获得的信号量id;
	    //参数2:内核链表中创建的信号id数组的下标;
	    //参数3:给信号量赋的初始值0/1;
    init_sem(semid, 0, 1);

    /*3.使用共享内存*/
	// 3.1操作资源前对资源上锁(p操作)
    sem_p(semid, 0, 1);

	    ///...FIXME!!!

	// 3.2结束操作资源后对资源解锁(v操作)
    sem_v(semid, 0, 1);
    /*4.撤销用户空间的映射*/
    if(shmdt((void *)pshm) < 0){
	perror("shmdt failed!");
	exit(1);
    }

    /*5.删除内核中创建的共享内存*/
    if(shmctl(shmid, IPC_RMID, NULL) < 0){
	perror("shmctl failed!");
	exit(1);
    }

    del_sem(semid);	//删除信号量id
//    del_sem(semid, 0, 0);
    return  0;
}
