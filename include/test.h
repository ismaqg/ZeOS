#ifndef __TEST_H__
#define __TEST_H__

int write_success(void);
int write_fails_invalid_file_descriptor(void);
int pthread_create_success(void);
int pthread_exit_success(void);
int pthread_join_success(void);
int mutex_init_success(void);
int mutex_destroy_success(void);
int mutex_lock_success(void);
int mutex_unlock_success(void);
int pthread_key_create_success(void);
int pthread_key_delete_success(void);
int pthread_getspecific_success(void);
int pthread_setspecific_success(void);
int errno_success(void);

#endif /* __TEST_H__ */