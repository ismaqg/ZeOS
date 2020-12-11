#include <libc.h>

int shared_vector[12];
int shared_index;
int shared_mutex;

void tear_down(int *TIDS, int TIDS_size, int mutex_id)
{
    for (int i = 0; i < TIDS_size; i++)
        pthread_join(TIDS[i], NULL);

    mutex_unlock(mutex_id);
    mutex_destroy(mutex_id);
}

void *modify_shared_vector(void *arg)
{
    int ret = -1;

    while (shared_index < 12)
    {
        ret = mutex_lock(shared_mutex);
        if (ret < 0)
            return (void *)0;

        shared_vector[shared_index] = (int)arg;
        shared_index++;

        ret = mutex_unlock(shared_mutex);
        if (ret < 0)
            return (void *)0;
    }

    return (void *)0;
}

int mutex_lock_success(void)
{
    int ret = -1;

    shared_mutex = mutex_init();
    if (shared_mutex < 0)
        return false;

    int TID, TIDS[2];

    ret = pthread_create(&TID, &modify_shared_vector, (void *)21);
    if (ret < 0)
        return false;

    TIDS[0] = TID;

    ret = pthread_create(&TID, &modify_shared_vector, (void *)11);
    if (ret < 0)
    {
        // Free all the created threads and mutexes in order to execute other tests correctly
        tear_down(TIDS, 1, shared_mutex);
        return false;
    }

    TIDS[1] = TID;

    int wait_for_threads = true;

    while (shared_index < 12)
    {
        ret = mutex_lock(shared_mutex);
        if (ret < 0)
        {
            // Free all the created threads and mutexes in order to execute other tests correctly
            tear_down(TIDS, 2, shared_mutex);
            return false;
        }

        shared_vector[shared_index] = 31;
        shared_index++;

        // Let enough time at first in order that the other threads start execution and get blocked at the mutex
        if (wait_for_threads)
        {
            wait_for_threads = false;
            delay(100);
        }

        ret = mutex_unlock(shared_mutex);
        if (ret < 0)
        {
            // Free all the created threads and mutexes in order to execute other tests correctly
            tear_down(TIDS, 2, shared_mutex);
            return false;
        }
    }

    for (int i = 0; i < 12; i++)
    {
        switch (i % 3)
        {
        case 0:
            if (shared_vector[i] != 31)
            {
                // Free all the created threads and mutexes in order to execute other tests correctly
                tear_down(TIDS, 2, shared_mutex);
                return false;
            }
            break;
        case 1:
            if (shared_vector[i] != 21)
            {
                // Free all the created threads and mutexes in order to execute other tests correctly
                tear_down(TIDS, 2, shared_mutex);
                return false;
            }
            break;
        case 2:
            if (shared_vector[i] != 11)
            {
                // Free all the created threads and mutexes in order to execute other tests correctly
                tear_down(TIDS, 2, shared_mutex);
                return false;
            }
            break;
        default:
            break;
        }
    }

    // Free all the created threads and mutexes in order to execute other tests correctly
    tear_down(TIDS, 2, shared_mutex);

    return true;
}

int mutex_lock_EINVAL(void)
{
    int ret = -1;
    int mutex_id;

    mutex_id = mutex_init();
    if (mutex_id < 0)
        return false;

    ret = mutex_destroy(mutex_id);
    if (ret < 0)
        return false;

    ret = mutex_lock(mutex_id);
    if (ret >= 0 || errno != EINVAL)
        return false;

    ret = mutex_lock(11);
    if (ret >= 0 || errno != EINVAL)
        return false;

    ret = mutex_lock(-31);
    if (ret >= 0 || errno != EINVAL)
        return false;

    ret = mutex_lock(31);
    if (ret >= 0 || errno != EINVAL)
        return false;

    // Unlock and destroy initialized mutex in order to execute other tests correctly
    mutex_unlock(mutex_id);
    mutex_destroy(mutex_id);

    return true;
}

int mutex_lock_EDEADLK(void)
{
    int ret = -1;
    int mutex_id;

    mutex_id = mutex_init();
    if (mutex_id < 0)
        return false;

    ret = mutex_lock(mutex_id);
    if (ret < 0)
        return false;

    ret = mutex_lock(mutex_id);
    if (ret >= 0 || errno != EDEADLK)
        return false;

    // Unlock and destroy initialized mutex in order to execute other tests correctly
    mutex_unlock(mutex_id);
    mutex_destroy(mutex_id);

    return true;
}
