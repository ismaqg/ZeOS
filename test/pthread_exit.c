#include <libc.h>

void *call_pthread_exit(void *arg)
{
	int ret = 31;

	pthread_exit(ret);

	ret = 21;

	pthread_exit(ret);

	ret = 11;

	return (void *)ret;
}

void *call_pthread_create(void *arg)
{
	int ret = -1;
	int TID, retval;

	ret = pthread_create(&TID, &call_pthread_exit, NULL);
	if (ret < 0 || TID <= 0)
		return false;

	ret = pthread_join(TID, &retval);
	if (ret < 0 || retval != 31)
		return false;

	ret = 31;

	pthread_exit(ret);

	ret = 21;

	pthread_exit(ret);

	ret = 11;

	return (void *)ret;
}

int pthread_exit_success(void)
{
	int ret = -1;
	int TID, retval;

	ret = pthread_create(&TID, &call_pthread_create, NULL);
	if (ret < 0 || TID <= 0)
		return false;

	ret = pthread_join(TID, &retval);
	if (ret < 0 || retval != 31)
		return false;

	return true;
}
