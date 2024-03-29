#include "shm.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

const char *shm_dir = "/dev/shm/";
const size_t shm_dirlen = 9;

/* adapted function from musl project, src/mman/shm_open.c
 * and from libc sysdeps/posix/shm-directory.h */
char *shm_mapname(const char *name, char *buf) {
    assert(name[0] == '/');
    /* Construct the filename.  */
    while (name[0] == '/') ++name;
    size_t namelen = strlen(name) + 1;
    /* Validate the filename.  */
    if (namelen == 1 || namelen >= SHM_NAME_MAXLEN ||
        strchr(name, '/') != NULL) {
        assert(0 && "invalid shm name");
        return 0;
    }
    if (SHM_NAME_MAXLEN <= shm_dirlen + namelen) {
        assert(0 && "buffer too short");
        return 0;
    }
    memcpy(buf, shm_dir, shm_dirlen);
    memcpy(buf + shm_dirlen, name, namelen);
    buf[shm_dirlen + namelen] = '\0';

    return buf;
}

char *shamon_map_ctrl_key(const char *buffkey, char key[SHM_NAME_MAXLEN]) {
    const size_t tmplen = strlen(buffkey);
    assert(tmplen < SHM_NAME_MAXLEN - 6);
    memcpy(key, buffkey, tmplen);
    memcpy(key + tmplen, ".ctrl", 6);
    /* we must have copied also the nul character */
    assert(key[tmplen + 5] == '\0');

    return key;
}

int shamon_shm_open(const char *key, int flags, mode_t mode) {
    char name[SHM_NAME_MAXLEN];
    if (shm_mapname(key, name) == 0)
        abort();
    return open(name, flags | O_NOFOLLOW | O_CLOEXEC | O_NONBLOCK, mode);
}

int shamon_get_tmp_key(const char *key, char *buf, size_t bufsize) {
    if (strlen(key) + 5 > bufsize) {
        return -1;
    }

    strcat(strcat(buf, key), ".tmp");

    return 0;
}

int shamon_shm_rename(const char *old_key, const char *new_key) {
    char old_name[SHM_NAME_MAXLEN];
    char new_name[SHM_NAME_MAXLEN];
    if (shm_mapname(old_key, old_name) == 0) {
        return -1;
    }
    if (shm_mapname(new_key, new_name) == 0) {
        return -1;
    }
    return rename(old_name, new_name);
}

int shamon_shm_unlink(const char *key) {
    printf("UNLINK: %s\n", key);
    char name[SHM_NAME_MAXLEN];
    if (shm_mapname(key, name) == 0)
        abort();
    return unlink(name);
}
