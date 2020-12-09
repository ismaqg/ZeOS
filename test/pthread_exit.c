#include <libc.h>

void *otra_funcion()
{
	println("soy el thread 2 y voy a hacer exit");
	pthread_exit(NULL);
	println("ESTO NUNCA DEBERÍA ESCRIBIRSE");
	return NULL;
}

void *funcion_nuevo_thread(void *argumento)
{
	printvar((int)argumento);
	/*while(1){
		println("soy el thread 1");
		for(int i = 0; i<10000000; i++);
       		for(int i = 0; i<10000000; i++);
        	for(int i = 0; i<10000000; i++);
	}*/
	int tid_nuevo_thread;
	pthread_create(&tid_nuevo_thread, &otra_funcion, NULL);
	println("soy el thread 1 y voy a hacer exit");
	pthread_exit(NULL);
	println("ESTO NUNCA DEBERÍA ESCRIBIRSE");
	return NULL;
}

int pthread_exit_success(void)
{

	int tid_nuevo_thread;
	pthread_create(&tid_nuevo_thread, &funcion_nuevo_thread, (void *)5);

	while (1)
	{
		println("soy el thread 0");
		for (int i = 0; i < 10000000; i++)
			;
		for (int i = 0; i < 10000000; i++)
			;
		for (int i = 0; i < 10000000; i++)
			;
	}

	return true;
}
