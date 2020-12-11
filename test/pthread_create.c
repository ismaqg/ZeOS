#include <libc.h>

void *return_arg(void *arg)
{
    return arg;
}

int pthread_create_success(void)
{
    int ret = -1;
    int TID, retval;

    for (int i = 0; i < 5; i++)
    {
        ret = pthread_create(&TID, &return_arg, (void *)(31 + i));
        if (ret < 0 || TID <= 0)
            return false;

        ret = pthread_join(TID, &retval);
        if (ret < 0 || retval != 31 + i)
            return false;
    }

    return true;
}

int pthread_create_EFAULT(void)
{
    int ret = -1;
    int TID;

    ret = pthread_create(NULL, &return_arg, NULL);
    if (ret >= 0 || errno != EFAULT)
        return false;

    ret = pthread_create((int *)&return_arg, &return_arg, NULL);
    if (ret >= 0 || errno != EFAULT)
        return false;

    ret = pthread_create(&TID, NULL, NULL);
    if (ret >= 0 || errno != EFAULT)
        return false;

    return true;
}

int pthread_create_EAGAIN(void)
{
    int ret = -1;
    int TID, TIDS[10];

    for (int i = 0; i < 10; i++)
    {
        ret = pthread_create(&TID, &return_arg, NULL);

        if (i < 8) // 2 task_structs are already being used, idle_task and task1
        {
            if (ret < 0 || TID <= 0)
                return false;

            TIDS[i] = TID;
        }
        else
        {
            if (ret >= 0 || errno != EAGAIN)
                return false;

            TIDS[i] = -1;
        }
    }

    // Free all the created threads in order to execute other tests correctly
    for (int i = 0; i < 10; i++)
    {
        if (TIDS[i] != -1)
        {
            pthread_join(TIDS[i], NULL);
        }
    }

    return true;
}