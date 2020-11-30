#include <libc.h>

char buff[256];

int write_success(void)
{
    int ret = write(1, "HOLA", 4);

    return (ret == 4 ? true : false);
}

int write_fails_invalid_file_descriptor(void)
{
    int ret = write(-31, "HOLA", 4);

    if (ret == -1 && errno == EBADF)
    {
        return true;
    }

    return false;
}