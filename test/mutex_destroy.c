#include <libc.h>

int mutex_destroy_success(void)
{
    int ret = -1;
    int mutex_id;

    for (int i = 0; i < 1000; i++)
    {
        mutex_id = mutex_init();
        if (mutex_id < 0)
            return false;

        ret = mutex_destroy(mutex_id);
        if (ret < 0)
            return false;
    }

    // isma: Si llegamos a este punto es que se han podido crear y deletear
    // 1000 mutexes (deleteando una justo despues de su creacion). Nota que
    // el maximo numero de mutex en el sistema es de 20

    return true;
}

int mutex_destroy_EINVAL(void)
{
    int ret = -1;
    int mutex_id;

    mutex_id = mutex_init();
    if (mutex_id < 0)
        return false;

    ret = mutex_destroy(mutex_id);
    if (ret < 0)
        return false;

    ret = mutex_destroy(mutex_id);
    if (ret >= 0 || errno != EINVAL)
        return false;

    ret = mutex_destroy(11);
    if (ret >= 0 || errno != EINVAL)
        return false;

    ret = mutex_destroy(-31);
    if (ret >= 0 || errno != EINVAL)
        return false;

    ret = mutex_destroy(31);
    if (ret >= 0 || errno != EINVAL)
        return false;

    return true;
}

int mutex_destroy_EBUSY(void)
{
    int ret = -1;
    int mutex_id;

    mutex_id = mutex_init();
    if (mutex_id < 0)
        return false;

    ret = mutex_lock(mutex_id);
    if (ret < 0)
        return false;

    ret = mutex_destroy(mutex_id);
    if (ret >= 0 || errno != EBUSY)
        return false;

    // Unlock and destroy initialized mutex in order to execute other tests correctly
    mutex_unlock(mutex_id);

    ret = mutex_destroy(mutex_id);
    if (ret < 0)
        return false;

    return true;
}

int mutex_destroy_EPERM(void)
{
    int ret = -1;

    ret = fork();
    if (ret < 0)
        return false;

    if (ret == 0)
    {
        int mutex_id = mutex_init();
        delay(500);
        mutex_destroy(mutex_id);
        exit();
    }
    else
    {
        delay(250);
        ret = mutex_destroy(0);
        if (ret >= 0 || errno != EPERM)
            return false;
    }

    return true;
}