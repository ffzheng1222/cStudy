#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
//#include <sys/ipc.h>
#include <semaphore.h>

int lock_var;  /* 保护的资源 */
pthread_mutex_t m; /* 互斥锁的动态定义*/

time_t end_time;


void pthread1 (void *arg);
void pthread2 (void *arg);

int main (int argc, char *argv[])
{
	pthread_t id1, id2;
	//pthread_t mon_th_id;
	int ret;

	end_time = time (NULL) + 30;

	pthread_mutexattr_t mutexattr;
	pthread_mutexattr_settype(&mutexattr,  PTHREAD_MUTEX_NORMAL);       

	pthread_mutex_init(&m, &mutexattr);
        pthread_mutexattr_destroy(&mutexattr);
	
	ret = pthread_create (&id1, NULL, (void *) pthread1, NULL);
	if (ret != 0)
		perror ("pthread cread1");

	ret = pthread_create (&id2, NULL, (void *) pthread2, NULL);
	if (ret != 0)
		perror ("pthread cread2");

	pthread_join (id1, NULL);
	pthread_join (id2, NULL);

	exit (0);
}

void pthread1 (void *arg)
{
	int i;
	printf("pthread1\n");

	while (time (NULL) < end_time) {
		printf ("pthread1: before sem_wait\n");
		
		pthread_mutex_lock(&m);  //加锁
		printf ("pthread1: after sem_wait()\n");
		for (i = 0; i < 2; i++) {
			sleep (1);
			lock_var++;
			printf ("lock_var=%d\n", lock_var);
		}

		printf ("pthread1:lock_var=%d\n", lock_var);

		pthread_mutex_unlock(&m);  //解锁
		sleep (1);
	}
}

void pthread2 (void *arg)
{
//  int nolock=0;
	//int ret;
	printf("pthread2\n");

	while (time (NULL) < end_time) {
		printf ("pthread2: before sem_wait()\n");
		
		pthread_mutex_lock(&m);  //加锁


		printf ("pthread2:pthread2 got lock;lock_var=%d\n", lock_var);

		 pthread_mutex_unlock(&m);  //解锁

		usleep (200000);  //0.2s
	}
}
