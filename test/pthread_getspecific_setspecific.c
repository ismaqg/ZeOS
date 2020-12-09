#include <libc.h>

void* funcion_testeo(void* arg){
	int key = pthread_key_create();
	int e = pthread_setspecific(key, arg);
	if (e < 0)
		return false;
	void* ret = pthread_getspecific(key);

	return ret;
}

int pthread_getspecific_setspecific_success(void)
{
	int key = pthread_key_create();
	int e = pthread_setspecific(key, (void *)123);
	if (e < 0)
		return false;
	void *ret1 = pthread_getspecific(key);

	int TID, ret2;
	pthread_create(&TID, funcion_testeo, (void*) 555);
	pthread_join(TID, &ret2);

	return (ret1 == (void *)123 && ret2 == 555);
}

int pthread_getspecific_setspecific_EINVAL(void){

	int e;

	e = pthread_setspecific(-1, (void*)123);
	if(e != -1 || errno != EINVAL) return false;
	e = pthread_setspecific(70, (void*)123);
	if(e != -1 || errno != EINVAL) return false;

	e = (int)pthread_getspecific(-1);
	if(e != -1 || errno != EINVAL) return false;
	e = (int)pthread_getspecific(70);
	if(e != -1 || errno != EINVAL) return false;



	int key = pthread_key_create();
	pthread_key_delete(key);

	e = pthread_setspecific(key, (void*)123);
	if(e != -1 || errno != EINVAL) return false;

	e = (int)pthread_getspecific(key);
	if(e != -1 || errno != EINVAL) return false;


	return true; //todo ha funcionado como esperábamos si llegamos aqui

}
