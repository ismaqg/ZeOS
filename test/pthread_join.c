#include <libc.h>

void hola(int argumento){
	if(argumento == 3){
		while(1); //NOTA QUE THREAD3 NO MORIRA
	}
	else{ print("\nsoy el thread "); printvar(argumento);}
	pthread_exit(argumento);
	println("ESTO NUNCA DEBERÍA ESCRIBIRSE");
}

int pthread_join_success(void)
{
    int thread1_TID, thread2_TID, thread3_TID;
    int thread1_retval, thread2_retval, thread3_retval;

    pthread_create(&thread1_TID, hola, 1);
    pthread_create(&thread2_TID, hola, 2);

    pthread_join(thread2_TID, &thread2_retval);
    pthread_join(thread1_TID, &thread1_retval);

    print("\nthread 2 ha acabado con retval: "); printvar(thread2_retval);
    print("\nthread 1 ha acabado con retval: "); printvar(thread1_retval);

    pthread_create(&thread3_TID, hola, 3);

    pthread_join(thread3_TID, &thread3_retval);

    println("ESTO NO DEBERÍA IMPRIMIRSE porque thread3 no acabará así que masterthread se bloquea");

    while(1);
    return true;
}
