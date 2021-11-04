#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHM_KEY_PATH "/"
#define SHM_KEY_PROJ_ID 'a'
#define SHM_SIZE 2048

int main(void)
{
    
    int shmid = -1;
    char *pshm = NULL;
    printf("Share Memory demo!\n");
# if 0
    //1.直接指定键值
    shmid = shmget(0x12341234, );
    
    //3. 申请私有键值 
    shmid = shmget(IPC_PRIVATE,  );
#endif
    
    printf("before shmget()\n");
    system("ipcs -m");

   //2. 通过ftok()
    key_t key;
    if( (key = ftok(SHM_KEY_PATH, SHM_KEY_PROJ_ID)) < 0) {
	perror("ftok");
	exit(1);
    }
    /*1. 创建或关联共享内存*/
    shmid = shmget(key, SHM_SIZE, IPC_CREAT|0666); /*若共享内存已经存在就关联上，不存在就以0666权限去创建*/
    if(shmid < 0) {
        perror("shmget");
        exit(1);
    }

    printf("after shmget()\n");
    system("ipcs -m");
   
    /*2.把内核中的共享内存区域映射到用户空间  */
    if((pshm = (char *)shmat(shmid, NULL, 0)) == (void *)-1) {
	perror("shmat");
	exit(1);
    }

    printf("after shmat()\n");
    system("ipcs -m");
    
    /*3. 使用 */
    //...FIXME!!!

    /*4.撤销映射 */
    shmdt((void *)pshm);
    
    printf("after shmdt()\n");
    system("ipcs -m");

    /*5. 删除共享内存*/
    shmctl(shmid, IPC_RMID, NULL);
    
    printf("after shmctl()\n");
    system("ipcs -m");

    return 0;
}
