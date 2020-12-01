#include <libc.h>

int mutex_unlock_success(void)
{
    int ret = mutex_unlock(NULL);

    return (ret == 42);
}