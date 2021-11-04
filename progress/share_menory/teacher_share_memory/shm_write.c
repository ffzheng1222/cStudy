#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define QUIT_STR  "quit"
#define SHM_KEY_PATH "/"
#define SHM_KEY_PROJ_ID 'a'
#define SHM_SIZE 2048

int main(void)
{
    
    int shmid = -1;
    char *pshm = NULL;
    printf("Share Memory demo!\n");

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

   
    /*2.把内核中的共享内存区域映射到用户空间  */
    if((pshm = (char *)shmat(shmid, NULL, 0)) == (void *)-1) {
	perror("shmat");
	exit(1);
    }
    
    /*3. 使用 */
    //...FIXME!!!
#if 1
      while(1) {
        /* 从标准键盘上获取数据 */
        fprintf(stderr, "please input string:");
  //      bzero(pshm, SHM_SIZE);
        if( fgets(pshm, SHM_SIZE-1, stdin) == NULL) {
          perror("fgets");
          continue;
        }

        if(!strncasecmp(pshm, QUIT_STR, strlen(QUIT_STR))) {   //用户输入了quit
            break;
        }
      }

#else 
     char buf[BUFSIZ];
     while(1) {
        /* 从标准键盘上获取数据 */
        fprintf(stderr, "please input string:");
        bzero(buf, BUFSIZ);
        if( fgets(buf, BUFSIZ-1, stdin) == NULL) {
          perror("fgets");
          continue;
        }
        /*写入管道 */
        do {
            ret = write(fd, buf, strlen(buf));
        }while(ret < 0 && EINTR ==errno);

        if(!strncasecmp(buf, QUIT_STR, strlen(QUIT_STR))) {   //用户输入了quit
            break;
        }
      }
#endif

    /*4.撤销映射 */
    shmdt((void *)pshm);
    

    /*5. 删除共享内存*/
    //shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
