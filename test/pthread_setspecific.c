#include <libc.h>

int pthread_setspecific_success(void)
{
    int ret = pthread_setspecific(NULL, NULL);

    return (ret == 46);
}