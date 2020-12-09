#include <libc.h>

int pthread_getspecific_setspecific_success(void)
{
	int key = pthread_key_create();
	int e = pthread_setspecific(key, (void *)123);
	if (e < 0)
		return false;
	void *ret = pthread_getspecific(key);

	// TODO (isma) : test it with more threads ensuring TLS is thread-specific

	return (ret == (void *)123);
}

int pthread_getspecific_setspecific_EINVAL(void)
{

	int e;

	e = pthread_setspecific(-1, (void *)123);
	if (e != -1 || errno != EINVAL)
		return false;
	e = pthread_setspecific(70, (void *)123);
	if (e != -1 || errno != EINVAL)
		return false;

	e = (int)pthread_getspecific(-1);
	if (e != -1 || errno != EINVAL)
		return false;
	e = (int)pthread_getspecific(70);
	if (e != -1 || errno != EINVAL)
		return false;

	int key = pthread_key_create();
	pthread_key_delete(key);

	e = pthread_setspecific(key, (void *)123);
	if (e != -1 || errno != EINVAL)
		return false;

	e = (int)pthread_getspecific(key);
	if (e != -1 || errno != EINVAL)
		return false;

	return true; //todo ha funcionado como esperábamos si llegamos aqui
}
