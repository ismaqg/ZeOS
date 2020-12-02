#include <libc.h>

int mutex_destroy_success(void)
{
    int ret = mutex_destroy(NULL);

    return (ret == 40);
}