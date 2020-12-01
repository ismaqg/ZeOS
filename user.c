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

  test("Write success", &write_success, true, true);
  test("Write fails invalid file descriptor", &write_fails_invalid_file_descriptor, true, false);
  test("Pthread_create success", &pthread_create_success, true, false);
  test("Pthread_exit success", &pthread_exit_success, true, false);
  test("Pthread_join success", &pthread_join_success, true, false);
  test("Mutex_init success", &mutex_init_success, true, false);
  test("Mutex_destroy success", &mutex_destroy_success, true, false);
  test("Mutex_lock success", &mutex_lock_success, true, false);
  test("Mutex_unlock success", &mutex_unlock_success, true, false);
  test("Pthread_key_create success", &pthread_key_create_success, true, false);
  test("Pthread_key_delete success", &pthread_key_delete_success, true, false);
  test("Pthread_getspecific success", &pthread_getspecific_success, true, false);
  test("Pthread_setspecific success", &pthread_setspecific_success, true, false);
  test("Modify errno", &errno_success, true, false);

  while (1)
  {
  }
}
