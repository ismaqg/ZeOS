#include <libc.h>

void *modify_errno(void *arg)
{
    int ret = -1;

    ret = fork();
    if (ret != -1)
        return 0;

    return (void *)errno;
}

int errno_success(void)
{
    int ret = -1;

    errno = 31;
    if (errno != 31)
        return false;

    int *perrno = (int *)0x109000;
    if (*perrno != 31)
        return false;

    *perrno = 21;
    if (*perrno != 21)
        return false;

    if (errno != 21)
        return false;

    int TID, retval;

    ret = pthread_create(&TID, &modify_errno, NULL);
    if (ret < 0 || TID <= 0)
        return false;

    ret = pthread_join(TID, &retval);
    if (ret < 0 || retval != EPERM || errno != 21)
        return false;

    return true;
}
