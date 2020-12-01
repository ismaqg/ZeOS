#include <libc.h>

int mutex_init_success(void)
{
    int ret = mutex_init();

    return (ret == 39);
}