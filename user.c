#include <libc.h>
#include <test.h>

char buff[256];

int pid;

int test(char *name, int (*func)(), int enabled, int output)
{
  if (enabled == false)
  {
    println("[SKIPPING] TEST : ");
    print(name);
    return -1;
  }

  if (output == true)
  {
    println("====== OUTPUT : ");
    print(name);
    print(" ======");
    println("");
  }
  else
  {
    println("");
  }

  int passed = func();
  
  if (output == true)
  {
    println("=======================");
    for (int i = 0; i < strlen(name); i++)
    {
      buff[i] = '=';
    }
    print(buff);
    println("");
  }

  if (passed == true)
  {
    print("[PASSED] TEST : ");
    print(name);
    return true;
  }

  print("[FAILED] TEST : ");
  print(name);
  return false;
}

int __attribute__((__section__(".text.main")))
main(void)
{
  /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
  /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

  // AS THERE IS NOT ENOUGH SPACE IN THE SCREEN FOR ALL THE TESTS OUTPUTS PLEASE ENABLE THE
  // TESTS IN BLOCKS OF 4 SO YOU CAN SEE THEM ALL (THIRD PARAMETER OF THE TEST FUNCTION)

  /* Write TestCase */
//  test("Write success", &write_success, true, true);
//  test("Write fails invalid file descriptor", &write_fails_invalid_file_descriptor, true, false);

  /* Fork TestCase */
//  test("Fork success", &fork_success, true, true);
//  test("Fork fails calling thread is not master", &fork_fails_calling_thread_is_not_master, true, false);

  /* Exit TestCase */
//  test("Exit success", &exit_success, true, true);
//  test("Exit fails calling thread is not master", &exit_fails_calling_thread_is_not_master, true, false);

  /* Pthread_create TestCase */
//  test("Pthread_create success", &pthread_create_success, true, false);

  /* Pthread_exit TestCase */
//  test("Pthread_exit success", &pthread_exit_success, true, false);

  /* Pthread_join TestCase */
//  test("Pthread_join success", &pthread_join_success, true, false);
//  test("Pthread_join EDEADLK", &pthread_join_EDEADLK, true, false);
//  test("Pthread_join EINVAL", &pthread_join_EINVAL, true, false);
//  test("Pthread_join ESRCH", &pthread_join_ESRCH, true, false);
//  test("Pthread_join EFAULT", &pthread_join_EFAULT, true, false);

  /* Mutex_init TestCase */
//  test("Mutex_init success", &mutex_init_success, true, false);

  /* Mutex_destroy TestCase */
//  test("Mutex_destroy success", &mutex_destroy_success, true, false);

  /* Mutex_lock TestCase */
//  test("Mutex_lock success", &mutex_lock_success, true, false);

  /* Mutex_unlock TestCase */
//  test("Mutex_unlock success", &mutex_unlock_success, true, false);

  /* Pthread_key_create TestCase */
//  test("Pthread_key_create success", &pthread_key_create_success, true, false);

  /* Pthread_key_delete TestCase */
//  test("Pthread_key_delete success", &pthread_key_delete_success, true, false);

  /* Pthread_getspecific TestCase */
//  test("Pthread_getspecific success", &pthread_getspecific_success, true, false);

  /* Pthread_setspecific TestCase */
//  test("Pthread_setspecific success", &pthread_setspecific_success, true, false);

  /* Errno TestCase */
//  test("Modify errno", &errno_success, true, false);

  while (1)
  {
  }
}
