#include "sem_comm.h"

/*
    信号量的初始化和删除都需要调用同一个函数semctl()
*/


/* 1. 初始化信号量*/
int init_sem(int semid, int sem_num, int init_val)
{
    union semun seun;
	seun.val = init_val;

    return semctl(semid, sem_num, SETVAL, seun);
}

/* 2. p(申请资源)操作（阻塞函数）*/
int sem_p(int semid, int sem_num, size_t nops)
{
    struct sembuf spf;
	spf.sem_num = sem_num;
	spf.sem_op = -1;
	spf.sem_flg = SEM_UNDO;

    return semop(semid, &spf, nops);
}

/* 2.1 p操作的非阻塞版本*/
int sem_non_p(int semid, int sem_num, size_t nops)
{
    struct sembuf nspf;
	nspf.sem_num = sem_num;
	nspf.sem_op = -1;
	nspf.sem_flg = IPC_NOWAIT|SEM_UNDO;

    return semop(semid, &nspf, nops); 
}

/* 3. v(释放资源)操作（非阻塞函数）*/
int sem_v(int semid, int sem_num, size_t nops)
{
    struct sembuf svf;
	svf.sem_num = sem_num;
	svf.sem_op = 1;
	svf.sem_flg = SEM_UNDO;

    return semop(semid, &svf, nops);    
}

/* 4. 删除信号量*/
#if 0
int del_sem(int semid, int sem_num, int init_val)
{
    union semun seun;
	seun.val = init_val;
    return semctl(semid, sem_num, IPC_RMID, seun); 
}
#else
int del_sem(int semid)
{
    union semun seun;
	
    return semctl(semid, 0, IPC_RMID, seun); 
}

#endif
