#include <libc.h>

int mutex_init_success_EAGAIN(void)
{
    int ret = -1;

    for (int i = 0; i < 25; i++)
    {
        ret = mutex_init();

        if (i < 20) // The maximum number of mutexes in the system is 20
        {
            if (ret < 0)
                return false;
        }
        else
        {
            if (ret >= 0 || errno != EAGAIN)
                return false;
        }
    }

    // Destroy mutexes initialized in order to execute other tests correctly
    for (int i = 0; i < 20; i++)
        mutex_destroy(i);

    return true;
}