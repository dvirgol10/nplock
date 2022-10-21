#include "nplock.h"


char* _generate_np_lock_name(np_lock_t *p_lock) {
    char *np_lock_name;
    if (asprintf(&np_lock_name, _NP_LOCK_NAME_FORMAT, _NP_LOCK_BASENAME, p_lock->id) == -1) {
        return NULL;
    }
    return np_lock_name;
}


int np_lock_init(np_lock_t *p_lock, int id) {
    char is_exist = 0;
    char *np_lock_name;

    p_lock->id = id;
    if (!(np_lock_name = _generate_np_lock_name(p_lock))) {
        fprintf(stderr, "asprintf\n");
        goto err;
    }

    if (mkfifo(np_lock_name, S_IRUSR | S_IWUSR) == -1) {
        if (errno == EEXIST) {
            is_exist = 1;
        } else {
            perror("mkfifo");
            goto err_free;
        }
    }

    if ((p_lock->fd_blocking = open(np_lock_name, O_RDWR)) == -1) {
        perror("open");
        goto err_free;
    }

    if ((p_lock->fd_non_blocking = open(np_lock_name, O_RDWR | O_NDELAY)) == -1) {
        perror("open");
        goto err_free;
    }

    if (!is_exist) {
        if (np_unlock(p_lock) == -1) {
            goto err;
        }
    }

    free(np_lock_name);
    return 0;

err_free:
    free(np_lock_name);
err:
    return -1;
}


int np_lock(np_lock_t *p_lock) {
    char c;
    if (read(p_lock->fd_blocking, &c, 1) == -1) {
        perror("read");
        return -1;
    }
    return 0;
}


int np_try_lock(np_lock_t *p_lock) {
    char c;
    ssize_t bytes_read = read(p_lock->fd_non_blocking, &c, 1);
    if (bytes_read == -1) {
        if (errno == EWOULDBLOCK) {
            bytes_read = 0;
        } else {
            perror("read");
            return -1;
        }
    }
    return bytes_read;
}


int np_unlock(np_lock_t *p_lock) {
    char c;
    if (write(p_lock->fd_blocking, &c, 1) == -1) {
        perror("write");
        return -1;
    }
    return 0;
}


int np_lock_destroy(np_lock_t *p_lock) {
    if (close(p_lock->fd_blocking) == -1) {
        perror("close");
        return -1;
    }
    
    if (close(p_lock->fd_non_blocking) == -1) {
        perror("close");
        return -1;
    }

    char *np_lock_name;
    if (!(np_lock_name = _generate_np_lock_name(p_lock))) {
        fprintf(stderr, "asprintf\n");
        return -1;
    }
    if (unlink(np_lock_name) == -1 && errno != ENOENT) {
        perror("unlink");
        free(np_lock_name);
        return -1;
    }
    free(np_lock_name);
    return 0;
}
