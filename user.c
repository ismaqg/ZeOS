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

  test("Write syscall call", &write_success, true, true);
  test("Write syscall invalid file descriptor", &write_fails_invalid_file_descriptor, true, false);

  while (1)
  {
  }
}
