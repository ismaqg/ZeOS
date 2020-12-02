#include <libc.h>

int pthread_join_success(void)
{
    int ret = pthread_join(NULL, NULL);

    return (ret == 38);
}