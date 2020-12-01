#include <libc.h>

int errno_success(void)
{
    errno = 31;
    if (errno != 31)
        return false;

    int *perrno = 0x109000;
    if (*perrno != 31)
        return false;

    *perrno = 21;
    if (*perrno != 21)
        return false;

    return true;
}