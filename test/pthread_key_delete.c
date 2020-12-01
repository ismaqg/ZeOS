#include <libc.h>

int pthread_key_delete_success(void)
{
    int ret = pthread_key_delete(NULL);

    return (ret == 44);
}