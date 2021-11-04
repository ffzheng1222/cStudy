#ifndef __LOCK_H__
#define __LOCK_H__

#include <unistd.h>
#include <sys/stat.h>
#include <syslog.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/file.h>
#include <stdio.h>
#include <sys/resource.h>
#include <signal.h>

int test_to_locked (char *path);

#endif
