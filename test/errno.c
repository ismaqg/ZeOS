#include <libc.h>

int errno_success(void)
{
    errno = 31;

    return (errno == 31);
}