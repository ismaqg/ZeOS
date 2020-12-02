#include <libc.h>

int pthread_getspecific_success(void)
{
    int ret = pthread_getspecific(NULL);

    return (ret == 45);
}