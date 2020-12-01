#include <libc.h>

int pthread_key_create_success(void)
{
    int ret = pthread_key_create();

    return (ret == 43);
}