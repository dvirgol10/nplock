#pragma once

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


#define _NP_LOCK_BASENAME "/tmp/nplock"
#define _NP_LOCK_NAME_FORMAT "%s.%d"


typedef struct np_lock_t {
    int id;
    int fd_blocking;
    int fd_non_blocking;
} np_lock_t;


int np_lock_init(np_lock_t *p_lock, int id);
int np_lock(np_lock_t *p_lock);
int np_try_lock(np_lock_t *p_lock);
int np_unlock(np_lock_t *p_lock);
int np_lock_destroy(np_lock_t *p_lock);
