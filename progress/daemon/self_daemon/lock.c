
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "debug.h"

/* 返回值： -1： 出错， 0： 锁已经被其它加锁了， 1： 成功*/
int test_to_locked (char *path)
{
	int fd;
	int ret = -1;
	struct flock lock;
	char *buf = "abdcefghijklmnopqrstuvwxyz";
	//begin to test file lock   
	fd = open (path, O_RDWR | O_CREAT, 0666);
	if (fd < 0) {
		perror ("Open file error");
		return ret;
	}

	if(write (fd, buf, strlen (buf)) < 0) 
	    return ret;
	fsync (fd);

	//file be locked?
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 10;
	lock.l_type = F_WRLCK;
	lock.l_pid = -1;
	if (fcntl (fd, F_GETLK, &lock) < 0) {
		perror ("fcntl failed!");
		return ret;
	}
//  if(lock.l_type == F_WRLCK)
	if (lock.l_type != F_UNLCK) {
		pr_debug("have a daemon. so quit!\n");
		return 0;
	}
	//locked file
	lock.l_type = F_WRLCK;
	if (fcntl (fd, F_SETLKW, &lock) < 0) {
		pr_debug("Lock file failed: type = %d\n", lock.l_type);
		return ret;
	}
	pr_debug("file have locked!\n");
	
	return 1;
}

