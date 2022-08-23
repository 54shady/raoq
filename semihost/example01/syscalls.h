#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include <sys/stat.h>

int _close(int fd);
int _isatty(int fd);
int _fstat(int fd, struct stat * st);
off_t _lseek(int fd, off_t ptr, int dir);
int _read(int fd, void *ptr, size_t len);
int _write(int fd, const char *ptr, size_t len);
int _getpid(void);
int _kill(int pid, int sig);

#endif
