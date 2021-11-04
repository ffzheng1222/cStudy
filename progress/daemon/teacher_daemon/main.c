#include <unistd.h>
#include "lock.h"

#include "debug.h"
#define LOCK_FILE "/tmp/mylock.lck"

int main (void)
{
        /* 进程守护进程化：
         *  1. 把当前的工作目录改为"/" 
         *  2.把stdin, stdout, stderr重定位到/dev/null [输入输出就没有了]
        */
	
        pr_debug("Daemonlize myself!\n");
        if(daemon(0, 0) < 0) {
	    perror("daemon");
	    exit(1);
	}
	pr_debug("Ha, you can not see me!\n");

	if( test_to_locked(LOCK_FILE) <= 0) {
	    exit(1);
	}
        openlog("Stephen", LOG_CONS|LOG_PID, LOG_DAEMON); 

	int i = 0;

	while (1) {
		syslog (LOG_ERR, "I am a daemon come from farsight, i=%d\n", i++);
		sleep (1);
	}
    
	closelog();

	return 0;
}
