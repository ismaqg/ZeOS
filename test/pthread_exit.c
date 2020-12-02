#include <libc.h>

int pthread_exit_success(void)
{
    pthread_exit(NULL);

    return true;
}