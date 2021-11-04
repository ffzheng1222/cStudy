#include "lock.h"
#include "debug.h"
#include <syslog.h>

#define LOCK_FILE "/tmp/mylock.lck"

int main(void)
{
    pr_debug("Daemonlize myself!\n");
    if(daemon(0,0) < 0)
    {
	perror("daemon failed!");
	exit(1);
    }
    pr_debug("you can't see to me!\n");

    if(test_to_locked(LOCK_FILE) <= 0)
    {
	exit(1);
    }

    openlog("fan",LOG_CONS|LOG_PID,LOG_DAEMON);

    int n = 10;
    while(n)
    {
	syslog(LOG_ERR,"I'am a daemon progress! n = %d\n",n--);
	sleep(1);
    }

    closelog();

    return 0;
}
