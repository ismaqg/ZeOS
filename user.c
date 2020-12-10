#include <libc.h>
#include <test.h>

char buff[256];

int pid;

int test(char *name, int (*func)(), int output)
{
  if (output == true)
  {
    println("====== OUTPUT : ");
    print(name);
    print(" ======");
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

  // AS THERE IS NOT ENOUGH SPACE IN THE SCREEN FOR ALL THE TESTS OUTPUTS PLEASE
  // ENABLE THE TESTS YOU WANT IN ORDER TO SEE THEM ALL (UNCOMMENT THEM).
  // IF YOU WANT TO SEE THE IMPLEMENTATION OF EACH TEST, JUST NAVIGATE TO:
  // test/<feature>.c

  /* Fork TestCase */
  // test("Fork success", &fork_success, true);
  // test("Fork EPERM", &fork_EPERM, false);

  /* Exit TestCase */
  // test("Exit success", &exit_success, true);
  // test("Exit ignore", &exit_ignore, false);

  /* Pthread_create TestCase */
  // test("Pthread_create success", &pthread_create_success, false);
  // test("Pthread_create EFAULT", &pthread_create_EFAULT, false);
  // test("Pthread_create EAGAIN", &pthread_create_EAGAIN, false);

  /* Pthread_exit TestCase */
  // test("Pthread_exit success", &pthread_exit_success, false);

  /* Pthread_join TestCase */
  // test("Pthread_join success", &pthread_join_success, false);
  // test("Pthread_join EDEADLK", &pthread_join_EDEADLK, false);
  // test("Pthread_join EINVAL", &pthread_join_EINVAL, false);
  // test("Pthread_join ESRCH", &pthread_join_ESRCH, false);
  // test("Pthread_join EFAULT", &pthread_join_EFAULT, false);

  /* Mutex_init TestCase */
  // test("Mutex_init success", &mutex_init_success, false);

  /* Mutex_destroy TestCase */
  // test("Mutex_destroy success", &mutex_destroy_success, false);

  /* Mutex_lock TestCase */
  // test("Mutex_lock success", &mutex_lock_success, false);

  /* Mutex_unlock TestCase */
  // test("Mutex_unlock success", &mutex_unlock_success, false);

  /* Pthread_key_create TestCase */
  // test("Pthread_key_create success and EAGAIN", &pthread_key_create_test, false);

  /* Pthread_key_delete TestCase */
  // test("Pthread_key_delete success", &pthread_key_delete_success, false);
  // test("Pthread_key_delete EINVAL", &pthread_key_delete_EINVAL, false);

  /* Pthread_getspecific and setspecific TestCase */
  // test("Pthread_getspecific and setspecific success", &pthread_getspecific_setspecific_success, false);
  // test("Pthread_setspecific and setspecific EINVAL", &pthread_getspecific_setspecific_EINVAL, false);

  /* Errno TestCase */
  // test("Individual errno", &errno_success, false);

  while (1)
  {
  }
}
