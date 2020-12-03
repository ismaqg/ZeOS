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

int exit_fails_calling_thread_is_not_master(void)
{
    return false;
}