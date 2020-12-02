#include <libc.h>

int pthread_getspecific_success(void)
{
    int ret = (int)pthread_getspecific(NULL);

    return (ret == 45);
}
