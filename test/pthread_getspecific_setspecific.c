#include <libc.h>

int pthread_setspecific_getspecific_success(void)
{
    int key = pthread_key_create();
    int e = pthread_setspecific(key, (void*)123);
    if(e < 0) return 0;
    void* ret = pthread_getspecific(key);

    return (ret == (void*)123);
}

int pthread_setspecific_getspecific_EINVAL(void){

	int e;

	e = pthread_setspecific(-1, (void*)123);
	if(e != -1 || errno != EINVAL) return 0;
	e = pthread_setspecific(70, (void*)123);
	if(e != -1 || errno != EINVAL) return 0;

	e = (int)pthread_getspecific(-1);
	if(e != -1 || errno != EINVAL) return 0;
	e = (int)pthread_getspecific(70);
	if(e != -1 || errno != EINVAL) return 0;



	int key = pthread_key_create();
	pthread_key_delete(key);

	e = pthread_setspecific(key, (void*)123);
	if(e != -1 || errno != EINVAL) return 0;

	e = (int)pthread_getspecific(key);
	if(e != -1 || errno != EINVAL) return 0;


	return 1; //todo ha funcionado como esperábamos si llegamos aqui

}
