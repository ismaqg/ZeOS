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

int fork_fails_calling_thread_is_not_master(void)
{
    return false;
}