#include <libc.h>

int fork_success(void)
{
    int ret = -1;

    if ((ret = fork()))
    {
        println("Hijo: ");
        printvar(ret);
        exit();
    }
    else
    {
        println("Padre: ");
        printvar(ret);
    }

    return (ret == 0);
}

void *call_fork(void *arg)
{
    int ret = -1;

    ret = fork();
    if (ret != -1)
        return (void *)0;

    return (void *)errno;
}

int fork_EPERM(void)
{
    int ret = -1;
    int TID, retval;

    ret = pthread_create(&TID, &call_fork, NULL);
    if (ret < 0 || TID <= 0)
        return false;

    ret = pthread_join(TID, &retval);
    if (ret < 0 || retval != EPERM)
        return false;

    return true;
}