#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include "sem_comm.h"

#define QUIT_STR  "quit"
#define SHM_KEY_PATH "/"
#define SHM_KEY_PROJ_ID 'a'
#define SEM_KEY_PATH "/"
#define SEM_KEY_PROJ_ID 'b'

#define SHM_SIZE 2048

int main(void)
{
    
    int shmid = -1, semid = -1;
    char *pshm = NULL;
    printf("Share Memory demo!\n");

    /*1. 创建或关联共享内存*/
    shmid = shmget(ftok(SHM_KEY_PATH, SHM_KEY_PROJ_ID), SHM_SIZE, IPC_CREAT|0666); 
    semid = semget(ftok(SEM_KEY_PATH, SEM_KEY_PROJ_ID), 1, IPC_CREAT|0666);

    if(shmid < 0 || semid < 0) {
        perror("shmget() or semget()");
        exit(1);
    }
   
    /*2.1 把内核中的共享内存区域映射到用户空间  */
    if((pshm = (char *)shmat(shmid, NULL, 0)) == (void *)-1) {
	perror("shmat");
	exit(1);
    }
    /*初始化信号量为起始可用 */
   // init_sem(semid, 0, 1);    
    
   /*3. 使用 */
     while(1) {
        /* 从共享内存中读数据 */
        sem_p(semid, 0, 1);
	if(strlen(pshm)) { 
	
	    printf("Reader: %s\n", pshm);
	    if(!strncasecmp(pshm, QUIT_STR, strlen(QUIT_STR))) {   //用户输入了quit
		sem_v(semid, 0, 1); //V操作
		 break;
	    }
	    bzero(pshm, SHM_SIZE);
	}
	sem_v(semid, 0, 1); //V操作

//	usleep(200000); //sleep 0.2s
      }

    /*4.撤销映射 */
    shmdt((void *)pshm);
    

    /*5. 删除共享内存和信号量 */
    shmctl(shmid, IPC_RMID, NULL);
    del_sem(semid);

    return 0;
}
