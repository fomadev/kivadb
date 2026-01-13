#include "kivadb_internal.h"
#include <io.h>
#include <sys/locking.h>

int kiva_lock_file(FILE* file) {
    int fd = fileno(file);
    _lseek(fd, 0L, SEEK_SET);
    // _LK_NBLCK : Verrouillage non-bloquant
    return _locking(fd, _LK_NBLCK, 1L);
}

void kiva_unlock_file(FILE* file) {
    int fd = fileno(file);
    _lseek(fd, 0L, SEEK_SET);
    _locking(fd, _LK_UNLCK, 1L);
}