/* adpUtentLib.c - utent replacement */

/* Copyright (c) 2008, TeamF1, Inc. */

/*
modification history
--------------------
01a,12apr08,chd  fixed UNLOCK issues for uClibc.
*/

/* utent.c <ndf@linux.mit.edu> */
/* Let it be known that this is very possibly the worst standard ever.  HP-UX
   does one thing, someone else does another, linux another... If anyone
   actually has the standard, please send it to me.

   Note that because of the way this stupid stupid standard works, you
   have to call endutent() to close the file even if you've not called
   setutent -- getutid and family use the same file descriptor. 

   Modified by Erik Andersen for uClibc...
*/

#include <features.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <paths.h>
#include <errno.h>
#include <string.h>



#ifdef __UCLIBC_HAS_THREADS__
#include <pthread.h>
static pthread_mutex_t utmplock = PTHREAD_MUTEX_INITIALIZER;
# define LOCK	pthread_mutex_lock(&utmplock)
# define UNLOCK	pthread_mutex_unlock(&utmplock);
#else
# define LOCK
# define UNLOCK
#endif

/* mappings */
#define setutent adpSetUtEnt
#define getutent adpGetUtEnt
#define endutent adpEndUtEnt
#include <utmp.h>


/* Some global crap */
static int static_fd = -1;
static struct utmp static_utmp;
static const char default_file_name[] = _PATH_UTMP;
static const char *static_ut_name = (const char *) default_file_name;



static struct utmp *__getutent(int utmp_fd)

{
    if (utmp_fd == -1) {
	setutent();
    }
    if (utmp_fd == -1) {
	return NULL;
    }

    LOCK;
    if (read(utmp_fd, (char *) &static_utmp, sizeof(struct utmp)) != sizeof(struct utmp)) 
    {
    UNLOCK;
	return NULL;
    }

    UNLOCK;
    return &static_utmp;
}

void setutent(void)
{
    int ret;

    LOCK;
    if (static_fd == -1) {
	if ((static_fd = open(static_ut_name, O_RDWR)) < 0) {
	    if ((static_fd = open(static_ut_name, O_RDONLY)) < 0) {
		goto bummer;
	    }
	}
	/* Make sure the file will be closed on exec()  */
	ret = fcntl(static_fd, F_GETFD, 0);
	if (ret >= 0) {
	    ret = fcntl(static_fd, F_GETFD, 0);
	}
	if (ret < 0) {
bummer:
	    UNLOCK;
	    static_fd = -1;
	    close(static_fd);
	    return;
	}
    }
    lseek(static_fd, 0, SEEK_SET);
    UNLOCK;
    return;
}

void endutent(void)
{
    LOCK;
    if (static_fd != -1) {
	close(static_fd);
    }
    static_fd = -1;
    UNLOCK;
}

/* Locking is done in __getutent */
struct utmp *getutent(void)
{
    return __getutent(static_fd);
}

#if 0
struct utmp * getutent(void)
{
    struct utmp *result;
    static struct utmp buffer;

    if (getutent_r(&buffer, &result) < 0)
	return NULL;

    return result;
}

/* Locking is done in __getutent */
struct utmp *getutid (const struct utmp *utmp_entry)
{
    struct utmp *lutmp;

    while ((lutmp = __getutent(static_fd)) != NULL) {
	if (	(utmp_entry->ut_type == RUN_LVL ||
		 utmp_entry->ut_type == BOOT_TIME ||
		 utmp_entry->ut_type == NEW_TIME ||
		 utmp_entry->ut_type == OLD_TIME) &&
		lutmp->ut_type == utmp_entry->ut_type)  
	{
	    return lutmp;
	}
	if (	(utmp_entry->ut_type == INIT_PROCESS ||
		 utmp_entry->ut_type == DEAD_PROCESS ||
		 utmp_entry->ut_type == LOGIN_PROCESS ||
		 utmp_entry->ut_type == USER_PROCESS) &&
		!strcmp(lutmp->ut_id, utmp_entry->ut_id)) 
	{
	    return lutmp;
	}
    }

    return NULL;
}

/* Locking is done in __getutent */
struct utmp *getutline(const struct utmp *utmp_entry)
{
    struct utmp *lutmp;

    while ((lutmp = __getutent(static_fd)) != NULL) {
	if ((lutmp->ut_type == USER_PROCESS || lutmp->ut_type == LOGIN_PROCESS) &&
		!strcmp(lutmp->ut_line, utmp_entry->ut_line))
	{
	    return lutmp;
	}
    }

    return NULL;
}

struct utmp *pututline (const struct utmp *utmp_entry)
{
    LOCK;
    /* Ignore the return value.  That way, if they've already positioned
       the file pointer where they want it, everything will work out. */
    lseek(static_fd, (off_t) - sizeof(struct utmp), SEEK_CUR);

    if (getutid(utmp_entry) != NULL) {
	lseek(static_fd, (off_t) - sizeof(struct utmp), SEEK_CUR);
	if (write(static_fd, utmp_entry, sizeof(struct utmp)) != sizeof(struct utmp))
	    return NULL;
    } else {
	lseek(static_fd, (off_t) 0, SEEK_END);
	if (write(static_fd, utmp_entry, sizeof(struct utmp)) != sizeof(struct utmp))
	    return NULL;
    }

    UNLOCK;
    return (struct utmp *)utmp_entry;
}

int utmpname (const char *new_ut_name)
{
    LOCK;
    if (new_ut_name != NULL) {
	if (static_ut_name != default_file_name)
	    free((char *)static_ut_name);
	static_ut_name = strdup(new_ut_name);
	if (static_ut_name == NULL) {
	    /* We should probably whine about out-of-memory 
	     * errors here...  Instead just reset to the default */
	    static_ut_name = default_file_name;
	}
    }

    if (static_fd != -1)
	close(static_fd);
    UNLOCK;
    return 0;
}
#endif

