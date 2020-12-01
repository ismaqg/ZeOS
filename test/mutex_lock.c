#include <libc.h>

int mutex_lock_success(void)
{
    int ret = mutex_lock(NULL);

    return (ret == 41);
}