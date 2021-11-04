#ifndef __SEM__COMM_H_
#define __SEM__COMM_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
};


//通过ftok函数获取共享内存的key值
#define SHM_KEY_PATH "/"
#define SHM_KEY_PROJ_ID 'a'

//通过ftok函数获取信号量的key值
#define SEM_KEY_PATH "/"
#define SEM_KEY_PROJ_ID 'b'

#define SHM_SIZE 2048

int init_sem(int semid, int sem_num, int init_val);
int sem_p(int semid, int sem_num, size_t nops);
int sem_non_p(int semid, int sem_num, size_t nops);
int sem_v(int semid, int sem_num, size_t nopos);
//int del_sem(int semid, int sem_num, int init_val);
int del_sem(int semid);


#endif
