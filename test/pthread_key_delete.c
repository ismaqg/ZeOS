#include <libc.h>

int pthread_key_delete_success(void)
{
	int ret = -1;
	int key;

	for (int i = 0; i < 1000; i++)
	{
		key = pthread_key_create();
		if (key < 0)
			return false;

		ret = pthread_key_delete(key);
		if (ret < 0)
			return false;
	}

	// isma: Si llegamos a este punto se han podido crear y deletear 1000 keys
	// (deleteando una justo despues de su creacion). Nota que TLS size es 64

	return true;
}

int pthread_key_delete_EINVAL(void)
{
	int ret = -1;
	int key;

	key = pthread_key_create();
	if (key < 0)
		return false;

	ret = pthread_key_delete(key);
	if (ret < 0)
		return false;

	ret = pthread_key_delete(key); // isma: Este debería fallar porque usa posicion ya desinicializada
	if (ret >= 0 || errno != EINVAL)
		return false;

	// isma: Si llegamos aquí es que el anterior delete ha retornado EINVAL, lo que queríamos.

	ret = pthread_key_delete(-6);
	if (ret >= 0 || errno != EINVAL)
		return false;

	ret = pthread_key_delete(70);
	if (ret >= 0 || errno != EINVAL)
		return false;

	// isma: Todo ha funcionado como esperábamos si llegamos aqui

	return true;
}
