#include <libc.h>

void *call_pthread_exit(void *arg)
{
	int ret = 31;

	pthread_exit(ret);
/*
	// if success, this point won't be reached.

	ret = 21;

	pthread_exit(ret);

	ret = 11;

	return (void *)ret;
*/
	println("Si se lee este mensaje, ha fallado pthread_exit");
	return NULL;
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
/*
	// if success, this point won't be reached.

	ret = 21;

	pthread_exit(ret);

	ret = 11;

	return (void *)ret;
*/
	println("Si se lee este mensaje, ha fallado pthread_exit");
	return NULL;
}

void* just_return(void* arg){
	return (void *)25;
}

/*EXPLICACIÓN TEST:
Se va a crear un thread que tiene que hacer exit con el valor 31. Ese thread también creará otro thread que también tendrá que hacer exit con el valor 31.
Si ambos threads no finalizan con el valor 31 significa que no hemos pasado el test.
Finalmente se creará otro thread que hará return 25; en su funcion (lo que implicitamente deberia actuar como un pthread_exit). Así que también comprobaremos si así ha ocurrido.
*/
int pthread_exit_success(void)
{
	//int ret = -1;
	int TID, retval;
/*
	ret = pthread_create(&TID, &call_pthread_create, NULL);
	if (ret < 0 || TID <= 0)
		return false;

	ret = pthread_join(TID, &retval);
	if (ret < 0 || retval != 31)
		return false;
*/
	pthread_create(&TID, &call_pthread_create, NULL);
	pthread_join(TID, &retval);
	if (retval != 31)
		return false;


	pthread_create(&TID, just_return, NULL);
	pthread_join(TID, &retval);
	if(retval != 25)
		return false;

	return true;
}
