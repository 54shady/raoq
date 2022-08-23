#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "pl011.h"

/* syscall stubs */
int _close(int fd)
{
    errno = EBADF;
    return -1;
}

int _isatty(int fd)
{
    return 1;
}

int _fstat(int fd, struct stat * st)
{
    errno = EBADF;
    return -1;
}

off_t _lseek(int fd, off_t ptr, int dir)
{
    errno = EBADF;
    return (off_t) -1;
}

int _read(int fd, void *ptr, size_t len)
{
    errno = EBADF;
    return -1;
}

int _write(int fd, const char *ptr, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        putchar_uart0(ptr[i]);
    }
    return len;
}

int _getpid(void)
{
    errno = EBADF;
    return -1;
}

int _kill(int pid, int sig)
{
    errno = EBADF;
    return -1;
}
