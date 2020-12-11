#include <libc.h>

int pthread_key_create_success_EAGAIN(void)
{
  int ret = -1;

  for (int i = 0; i < 100; i++)
  {
    ret = pthread_key_create();

    if (i < 64) // TLS size is 64
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

  // Clear TLS in order to execute other tests correctly
  for (int i = 0; i < 64; i++)
    pthread_key_delete(i);

  return true;
}
