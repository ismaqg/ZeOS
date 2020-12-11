#include <libc.h>

int exit_success(void)
{
    int ret = -1;

    if ((ret = fork()))
    {
        exit();
        println("Hijo: ");
        printvar(ret);
    }
    else
    {
        println("Padre: ");
        printvar(ret);
    }

    return (ret == 0);
}

void *call_exit(void *arg)
{
    int ret = 31;

    exit();

    ret = 21;

    return (void *)ret;
}

int exit_calling_thread_is_not_master(void)
{
    int ret = -1;
    int TID, retval;

    ret = pthread_create(&TID, &call_exit, NULL);
    if (ret < 0 || TID <= 0)
        return false;

    ret = pthread_join(TID, &retval);
    if (ret < 0 || retval != 21)
        return false;

    return true;
}