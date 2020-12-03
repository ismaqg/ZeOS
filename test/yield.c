#include <libc.h>

int yield_success(void)
{
    int ret = yield();

    return (ret == 4);
}
