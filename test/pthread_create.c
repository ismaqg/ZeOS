#include <libc.h>

int pthread_create_success(void)
{
    int ret = pthread_create(NULL, NULL, NULL);

    return (ret == 36);
}