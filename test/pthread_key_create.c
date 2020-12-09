#include <libc.h>

int pthread_key_create_test(void)
{

  int e = 0, contador_keys_cogidas = 0;
  while (e >= 0)
  {
    e = pthread_key_create();
    if (e >= 0)
      contador_keys_cogidas++;
  }

  return (errno == EAGAIN && contador_keys_cogidas == 64); // habra tenido exito el pthread_key_create si ha sido capaz de darle 64 keys (tamaño tls) al thread pero a la número 65 ha retornado error EAGAIN.
}
