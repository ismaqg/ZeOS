#include <libc.h>

int pthread_key_delete_success(void)
{
    int e, key;
    for(int i = 0; i < 1000; i++){
	key = pthread_key_create();
	if(key < 0){println("en el test del key delete ha fallado el key create"); break;} 
	e = pthread_key_delete(key);
	if(e != 0) break;
    }
    // si llegamos a este punto con e == 0 es que se han podido crear y deletear 1000 keys (deleteando una justo despues de su creacion). Nota que TLS size es 64

    return (e == 0); 
}

int pthread_key_delete_EINVAL(void){
	int key, e;
	key = pthread_key_create();
	e = pthread_key_delete(key);
	if(e < 0){
		println("ha fallado un key delete que deberia haber funcionado");
		return 0;
	}
	e = pthread_key_delete(key); //este debería fallar porque usa posicion ya desinicializada
	if(e != -1 || errno != EINVAL) return 0;

	//si llegamos aquí es que el anterior delete ha retornado EINVAL, lo que queríamos.

	e = pthread_key_delete(-6);
	if(e != -1 || errno != EINVAL) return 0;

	e = pthread_key_delete(70);
	if(e != -1 || errno != EINVAL) return 0;

	return 1; //todo ha funcionado como esperábamos si llegamos aqui
}
