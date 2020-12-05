#ifndef __TEST_H__
#define __TEST_H__

/* Write TestCase */
int write_success(void);
int write_fails_invalid_file_descriptor(void);

/* Fork TestCase */
int fork_success(void);
int fork_fails_calling_thread_is_not_master(void);

/* Exit TestCase */
int exit_success(void);
int exit_fails_calling_thread_is_not_master(void);

/* Pthread_create TestCase */
int pthread_create_success(void);

/* Pthread_exit TestCase */
int pthread_exit_success(void);

/* Pthread_join TestCase */
int pthread_join_success(void);

/* Mutex_init TestCase */
int mutex_init_success(void);

/* Mutex_destroy TestCase */
int mutex_destroy_success(void);

/* Mutex_lock TestCase */
int mutex_lock_success(void);

/* Mutex_unlock TestCase */
int mutex_unlock_success(void);

/* Pthread_key_create TestCase */
int pthread_key_create_success(void);

/* Pthread_key_delete TestCase */
int pthread_key_delete_success(void);

/* Pthread_getspecific TestCase */
int pthread_getspecific_success(void);

/* Pthread_setspecific TestCase */
int pthread_setspecific_success(void);

/* Errno TestCase */
int errno_success(void);

#endif /* __TEST_H__ */