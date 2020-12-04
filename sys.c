/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <p_stats.h>

#include <errno.h>

#define LECTURA 0
#define ESCRIPTURA 1

struct mutex_t mutexes[MAX_MUTEXES];

void *get_ebp();

int check_fd(int fd, int permissions)
{
  if (fd != 1)
    return -EBADF;
  if (permissions != ESCRIPTURA)
    return -EACCES;
  return 0;
}

void user_to_system(void)
{
  update_stats(&(current()->p_stats.user_ticks), &(current()->p_stats.elapsed_total_ticks));
}

void system_to_user(void)
{
  update_stats(&(current()->p_stats.system_ticks), &(current()->p_stats.elapsed_total_ticks));
}

int sys_ni_syscall()
{
  return -ENOSYS;
}

int sys_getpid()
{
  return current()->PID;
}

int global_PID = 1000;

int ret_from_fork()
{
  return 0;
}

int sys_fork(void)
{
  // Only the master thread of the current process can perform fork syscall
  if (current()->TID != 0)
    return -EPERM;

  struct list_head *lhcurrent = NULL;
  union task_union *uchild;

  /* Any free task_struct? */
  if (list_empty(&freequeue))
    return -ENOMEM;

  lhcurrent = list_first(&freequeue);

  list_del(lhcurrent);

  uchild = (union task_union *)list_head_to_task_struct(lhcurrent);

  /* Copy the parent's task struct to child's */
  copy_data(current(), uchild, sizeof(union task_union));

  /* new pages dir */
  allocate_DIR((struct task_struct *)uchild);

  /* Allocate pages for DATA+STACK */
  int new_ph_pag, pag, i;
  page_table_entry *process_PT = get_PT(&uchild->task);
  for (pag = 0; pag < NUM_PAG_DATA; pag++)
  {
    new_ph_pag = alloc_frame();
    if (new_ph_pag != -1) /* One page allocated */
    {
      set_ss_pag(process_PT, PAG_LOG_INIT_DATA + pag, new_ph_pag);
    }
    else /* No more free pages left. Deallocate everything */
    {
      /* Deallocate allocated pages. Up to pag. */
      for (i = 0; i < pag; i++)
      {
        free_frame(get_frame(process_PT, PAG_LOG_INIT_DATA + i));
        del_ss_pag(process_PT, PAG_LOG_INIT_DATA + i);
      }
      /* Deallocate task_struct */
      list_add_tail(lhcurrent, &freequeue);

      /* Return error */
      return -EAGAIN;
    }
  }

  /* Copy parent's SYSTEM and CODE to child. */
  page_table_entry *parent_PT = get_PT(current());
  for (pag = 0; pag < NUM_PAG_KERNEL; pag++)
  {
    set_ss_pag(process_PT, pag, get_frame(parent_PT, pag));
  }
  for (pag = 0; pag < NUM_PAG_CODE; pag++)
  {
    set_ss_pag(process_PT, PAG_LOG_INIT_CODE + pag, get_frame(parent_PT, PAG_LOG_INIT_CODE + pag));
  }
  /* Copy parent's DATA to child. We will use TOTAL_PAGES-1 as a temp logical page to map to */
  for (pag = NUM_PAG_KERNEL + NUM_PAG_CODE; pag < NUM_PAG_KERNEL + NUM_PAG_CODE + NUM_PAG_DATA; pag++)
  {
    /* Map one child page to parent's address space. */
    set_ss_pag(parent_PT, pag + NUM_PAG_DATA, get_frame(process_PT, pag));
    copy_data((void *)(pag << 12), (void *)((pag + NUM_PAG_DATA) << 12), PAGE_SIZE);
    del_ss_pag(parent_PT, pag + NUM_PAG_DATA);
  }
  /* Deny access to the child's memory space */
  set_cr3(get_DIR(current()));

  uchild->task.PID = ++global_PID;
  uchild->task.state = ST_READY;

  uchild->task.TID = 0;
  uchild->task.joined = NULL;
  uchild->task.errno = 0;
  uchild->task.retval = 0;
  init_tls(&(uchild->task.TLS));

  // Get a free threads_process list
  for (int i = 0; i < NR_TASKS; i++)
  {
    if (threads_processes[i].next == NULL)
    {
      uchild->task.threads_process = &(threads_processes[i]);
      break;
    }
  }

  INIT_LIST_HEAD(uchild->task.threads_process);
  list_add_tail(&(uchild->task.list_threads), uchild->task.threads_process);

  int register_ebp; /* frame pointer */
  /* Map Parent's ebp to child's stack */
  register_ebp = (int)get_ebp();
  register_ebp = (register_ebp - (int)current()) + (int)(uchild);

  uchild->task.register_esp = register_ebp + sizeof(DWord);

  DWord temp_ebp = *(DWord *)register_ebp;
  /* Prepare child stack for context switch */
  uchild->task.register_esp -= sizeof(DWord);
  *(DWord *)(uchild->task.register_esp) = (DWord)&ret_from_fork;
  uchild->task.register_esp -= sizeof(DWord);
  *(DWord *)(uchild->task.register_esp) = temp_ebp;

  /* Set stats to 0 */
  init_stats(&(uchild->task.p_stats));

  /* Queue child process into readyqueue */
  uchild->task.state = ST_READY;
  list_add_tail(&(uchild->task.list), &readyqueue);

  return uchild->task.PID;
}

#define TAM_BUFFER 512

int sys_write(int fd, char *buffer, int nbytes)
{
  char localbuffer[TAM_BUFFER];
  int bytes_left;
  int ret;

  if ((ret = check_fd(fd, ESCRIPTURA)))
    return ret;
  if (nbytes < 0)
    return -EINVAL;
  if (!access_ok(VERIFY_READ, buffer, nbytes))
    return -EFAULT;

  bytes_left = nbytes;
  while (bytes_left > TAM_BUFFER)
  {
    copy_from_user(buffer, localbuffer, TAM_BUFFER);
    ret = sys_write_console(localbuffer, TAM_BUFFER);
    bytes_left -= ret;
    buffer += ret;
  }
  if (bytes_left > 0)
  {
    copy_from_user(buffer, localbuffer, bytes_left);
    ret = sys_write_console(localbuffer, bytes_left);
    bytes_left -= ret;
  }
  return (nbytes - bytes_left);
}

extern int zeos_ticks;

int sys_gettime()
{
  return zeos_ticks;
}

void sys_exit()
{
  // Only the master thread of the current process can perform exit syscall
  if (current()->TID != 0)
    return;

  int threads_num = 0;

  /* Remove resources from all the threads of the current process */

  struct list_head *pos;
  list_for_each(pos, current()->threads_process)
  {
    struct task_struct *tmp = list_head_to_task_struct(pos);

    // TODO (isma) : mover esto a una funcion ya que la sys_mutex_destroy deberia hacer algo parecido no?
    for (int i = 0; i < MAX_MUTEXES; i++)
    {
      if (mutexes[i].pid_owner == tmp->PID && mutexes[i].tid_owner == tmp->TID)
      {
        mutexes[i].pid_owner = -1;
        mutexes[i].tid_owner = -1;
        list_del(&(mutexes[i].blockedqueue));
        mutexes[i].initialized = 0;
      }
    }

    tmp->PID = -1;
    tmp->TID = -1;
    list_add_tail(&(tmp->list), &freequeue); // Free task_struct

    threads_num++;
  }

  /* Remove resources of the current process */

  list_del(current()->threads_process); // TODO : REVISAR TODOS LOS list_del -> NO BORRAN LA LISTA SINO EL ELEMENTO EN LA LISTA!!!!
  // TODO : SI HAGO UN LIST_ADD_TAIL ACORDARSE DE HACER LIST_DEL ANTES!!!

  page_table_entry *process_PT = get_PT(current());

  // Deallocate all the propietary physical pages
  for (int i = 0; i < NUM_PAG_DATA + threads_num - 1; i++)
  {
    free_frame(get_frame(process_PT, PAG_LOG_INIT_DATA + i));
    del_ss_pag(process_PT, PAG_LOG_INIT_DATA + i);
  }

  // Restart execution of the next ready process
  switch (sched_next_decide_level())
  {
  case 2:
    sched_next_rr_level2();
    break;
  case 1: // TODO : delete debug
    panic("sys_exit switched to a thread of the same process");
    sched_next_rr_level1();
    break;
  default:
    // Switch to idle_task as the readyqueue is empty
    sched_next_rr(idle_task);
    break;
  }
}

/* System call to force a task switch */
int sys_yield()
{
  force_task_switch();
  return 0;
}

extern int remaining_quantum_process;
extern int remaining_quantum_thread;

int sys_get_stats(int pid, struct stats *st)
{
  int i;

  if (!access_ok(VERIFY_WRITE, st, sizeof(struct stats)))
    return -EFAULT;

  if (pid < 0)
    return -EINVAL;
  for (i = 0; i < NR_TASKS; i++)
  {
    if (task[i].task.PID == pid)
    {
      task[i].task.p_stats.remaining_ticks = remaining_quantum_process;
      copy_to_user(&(task[i].task.p_stats), st, sizeof(struct stats));
      return 0;
    }
  }
  return -ESRCH; /*ESRCH */
}

int sys_pthread_create(int *TID, void *(*start_routine)(), void *arg)
{
  int result = -1;

  // Check TID
  result = (TID == NULL ? -1 : 0);
  if (result < 0)
    return -EFAULT;

  result = (access_ok(VERIFY_WRITE, TID, sizeof(int)) ? 0 : -1);
  if (result < 0)
    return -EFAULT;

  // Check start_routine
  result = (start_routine == NULL ? -1 : 0);
  if (result < 0)
    return -EFAULT;

  // Check freequeue
  result = (list_empty(&freequeue) ? -1 : 0);
  if (result < 0)
    return -EAGAIN;

  /* Pick a free task */
  struct list_head *free_task = list_first(&freequeue);
  struct task_struct *new_task = list_head_to_task_struct(free_task);
  list_del(free_task); // Delete task from queue

  /* Initialize new task */
  union task_union *new_task_union = (union task_union *)new_task;      // Get the task_union for new_task
  union task_union *current_task_union = (union task_union *)current(); // Get the task_union for current task

  int threads_num = 0;

  struct list_head *pos;
  list_for_each(pos, current()->threads_process)
  {
    threads_num++;
  }

  /* Inherit system data */
  copy_data(current_task_union, new_task_union, sizeof(union task_union));

  /* Assign new TID */
  new_task->TID = threads_num;
  result = copy_to_user(&(new_task->TID), TID, sizeof(int)); // Copy new TID to *TID
  if (result < 0)
    return -EFAULT;

  /* Inherit user data */

  // Get the page table for new_task (same for current task)
  page_table_entry *new_page_table = get_PT(new_task);

  // Allocate new frame for new stack
  int new_stack_frame = alloc_frame();
  if (new_stack_frame < 0)
    return -ENOMEM;

  int current_stack_page = PAG_LOG_INIT_DATA + NUM_PAG_DATA + current()->TID;
  int new_stack_page = PAG_LOG_INIT_DATA + NUM_PAG_DATA + new_task->TID;

  set_ss_pag(new_page_table, new_stack_page, new_stack_frame);
  copy_data((int *)(current_stack_page << 12), (int *)(new_stack_page << 12), PAGE_SIZE); // TODO : MIRAR SI SE COPIA EL STACK DEL CURRENT AL NEW THREAD;

  /* Initialize task_struct structures */
  new_task->state = ST_READY;
  init_stats(&(new_task->p_stats));

  new_task->joined = NULL;
  new_task->errno = 0;
  new_task->retval = 0;
  init_tls(&(new_task->TLS));

  list_add_tail(&(new_task->list_threads), new_task->threads_process);

  // LA PILA DEL NUEVO THREAD DEBERIA SER ASI (MENOR A MAYOR):
  // ARG | @RET ?? <- APUNTAR HW ESP AQUI

  /* TODO : Prepare new_task context for task_switch INJECT THE ARG! INJECTAR EN EIP HW CAMBIAR ESP DEL CONTEXT HARDWARE POR LA BASE DE LA PILA NUEVA */
  // Get the position of the current EBP in the new_task system stack
  int ebp_index = KERNEL_STACK_SIZE - 18; // 5 HW CTX | 11 SW CTX | 1 @pthread_create_handler | 1 previous EBP
  // Mock EBP value
  new_task_union->stack[ebp_index - 1] = 0;
  // Inject start_routine function address
  new_task_union->stack[ebp_index] = (unsigned long)start_routine;

  // Point new_task register_esp to the faked EBP
  new_task->register_esp = &(new_task_union->stack[ebp_index - 1]);

  // Enqueue new process to readyqueue
  list_add_tail(&(new_task->list), &readyqueue);

  return 0;
}

void sys_pthread_exit(int retval)
{
}

int sys_pthread_join(int TID, int *retval)
{
  return 38;
}

int sys_mutex_init()
{
  return 39;
}

int sys_mutex_destroy(int mutex_id)
{
  return 40;
}

int sys_mutex_lock(int mutex_id)
{
  return 41;
}

int sys_mutex_unlock(int mutex_id)
{
  return 42;
}

int sys_pthread_key_create()
{
  return 43;
}

int sys_pthread_key_delete(int key)
{
  return 44;
}

void *sys_pthread_getspecific(int key)
{
  return (void *)45;
}

int sys_pthread_setspecific(int key, void *value)
{
  return 46;
}
