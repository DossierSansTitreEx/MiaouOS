#ifndef _UNISTD_H
#define _UNISTD_H 1

#include <sys/cdefs.h>

#include <sys/_types/_size_t.h>
#include <sys/_types/_ssize_t.h>

#ifndef __KERNEL
__BEGIN_DECL
ssize_t write(int fd, const void* buf, size_t len);
__END_DECL
#endif

#endif
