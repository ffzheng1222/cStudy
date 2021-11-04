#ifndef __SEM_COMM_H__
#define  __SEM_COMM_H__

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#ifndef semun
 union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
};
#endif

int init_sem(int semid, int sem_num, int init_val);
int sem_p(int semid, int sem_num, size_t nops);
int sem_try_p(int semid, int sem_num, size_t nops);
int sem_v(int semid, int sem_num, size_t nops);
int del_sem(int semid);
    
#endif
