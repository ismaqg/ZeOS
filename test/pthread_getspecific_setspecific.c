#include <libc.h>

void *call_pthread_getspecific_setspecific(void *arg)
{
	int ret = -1;
	int key;

	key = pthread_key_create();
	if (key < 0)
		return (void *)0;

	ret = pthread_setspecific(key, (void *)21);
	if (ret < 0)
		return (void *)0;

	void *val = pthread_getspecific(key);
	if ((int)val != 21)
		return (void *)0;

	return (void *)val;
}

int pthread_getspecific_setspecific_success(void)
{
	int ret = -1;
	int key;

	key = pthread_key_create();
	if (key < 0)
		return false;

	ret = pthread_setspecific(key, (void *)31);
	if (ret < 0)
		return false;

	void *val = pthread_getspecific(key);
	if ((int)val != 31)
		return false;

	int TID, retval;

	ret = pthread_create(&TID, &call_pthread_getspecific_setspecific, NULL);
	if (ret < 0 || TID <= 0)
		return false;

	ret = pthread_join(TID, &retval);
	if (ret < 0 || retval != 21)
		return false;

	return true;
}

int pthread_getspecific_setspecific_EINVAL(void)
{
	int ret = -1;

	ret = pthread_setspecific(-1, (void *)123);
	if (ret >= 0 || errno != EINVAL)
		return false;

	ret = pthread_setspecific(70, (void *)123);
	if (ret >= 0 || errno != EINVAL)
		return false;

	ret = (int)pthread_getspecific(-1);
	if (ret >= 0 || errno != EINVAL)
		return false;

	ret = (int)pthread_getspecific(70);
	if (ret >= 0 || errno != EINVAL)
		return false;

	int key = pthread_key_create();
	if (key < 0)
		return false;

	ret = pthread_key_delete(key);
	if (ret < 0)
		return false;

	ret = pthread_setspecific(key, (void *)123);
	if (ret >= 0 || errno != EINVAL)
		return false;

	ret = (int)pthread_getspecific(key);
	if (ret >= 0 || errno != EINVAL)
		return false;

	// isma : todo ha funcionado como esperábamos si llegamos aqui

	return true;
}
