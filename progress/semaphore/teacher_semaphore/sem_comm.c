#include "sem_comm.h"
//初始化
int init_sem(int semid, int sem_num, int init_val)
{
    union semun sun;
    sun.val = init_val;
    return semctl(semid,sem_num, SETVAL, sun); 	
}


//P操作，是阻塞函数 
int sem_p(int semid, int sem_num, size_t nops)
{
    struct sembuf st;
    
    st.sem_num = sem_num;
    st.sem_op = -1;
    st.sem_flg = SEM_UNDO;    
    return semop(semid, &st, nops);
}

//P操作的非阻塞版本
int sem_try_p(int semid, int sem_num, size_t nops)
{
    struct sembuf st;

    st.sem_num = sem_num;
    st.sem_op = -1;
    st.sem_flg = IPC_NOWAIT|SEM_UNDO;
    return semop(semid, &st, nops);
}

//v操作
int sem_v(int semid, int sem_num, size_t nops)
{
    struct sembuf st;

    st.sem_num = sem_num;
    st.sem_op = 1;
    st.sem_flg = SEM_UNDO;
    return semop(semid, &st, nops);
}

//删除
int del_sem(int semid)
{
    union semun sun;
    return semctl(semid, 0, IPC_RMID, sun);
}
    
