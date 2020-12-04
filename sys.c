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

// calculate the logical page number of the user_stack of the thread.
// it is based on that the masterthread (tid = 0) owns the 20th page of user data and that the TIDs are given sequentially.
#define calc_VPN_user_stack(TID) (PAG_LOG_INIT_DATA + 19 + TID)

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
  int i;

  page_table_entry *process_PT = get_PT(current());

  // Deallocate all the propietary physical pages
  for (i = 0; i < NUM_PAG_DATA; i++)
  {
    free_frame(get_frame(process_PT, PAG_LOG_INIT_DATA + i));
    del_ss_pag(process_PT, PAG_LOG_INIT_DATA + i);
  }

  /* Free task_struct */
  list_add_tail(&(current()->list), &freequeue);

  current()->PID = -1;

  /* Restarts execution of the next process */
  switch(sched_next_decide_level()){ //
	case 2:
		sched_next_rr_level2();
		break;
	case 1:
		println("ERROR: algun thread aun en ready al final de sys_exit");
		break;
	default: // empty ready_queue
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
  return 36;
}

void sys_pthread_exit(int retval)
{
	if(current()->TID == 0) // isma: si es el masterthread, que el pthread_exit se convierta en un exit
		sys_exit();


	struct task_struct* t;
        if((t = current()->joined) != NULL){ // isma: t es la &task_struct de quien hizo join conmigo. (NULL si nadie)
		t->state = ST_READY;
        	list_del(&(t->joined->list)); // isma: unblock the thread who joined me
		list_add_tail(&(t->joined->list), &readyqueue);
        }
	
	current()->retval = retval;
	current()->state = ST_ZOMBIE;

	struct list_head *pos;
	int threads_in_the_process_counter = 0;
	list_for_each(pos, current()->threads_process){ // isma: threads_process is a reference to the sentinel of the threads queue of a certain process
		struct task_struct *t_aux = list_head_to_task_struct(pos);
		if(t_aux->state != ST_ZOMBIE)
			threads_in_the_process_counter++;
	}

	if(threads_in_the_process_counter == 1){ // isma: solo quedo yo en el process (o los demás que quedan son zombies y nadie les había joineado)
		sys_exit();
	}
	else if(threads_in_the_process_counter > 1){
		switch(sched_next_decide_level()){ 
			case 2: // isma: the other threads of the same process are blocked but there are other processes in ready. El unico caso donde se me ocurre que puede ocurrir esto sin que sea un desastre absoluto es cuando estan haciendo cosas sobre un mutex junto a OTRO proceso, si no nada les podría desbloquear.
				sched_next_rr_level2(); 
				break;
			case 1: // isma: some other thread(s) of the same process is (are) ready
				sched_next_rr_level1();
				break;
			default: // empty ready_queue
				panic("ERROR pthread_exit: Nos hemos quedado en un sistema en el que todo está bloqueado");
				sched_next_rr(idle_task);
				break;
		  }
	}
	else{
		panic("error thread_exit: el contador de threads dice que hay 0 threads en el proceso\n");
	}

	// isma: Los recursos del thread no serán liberados hasta que otro thread haga join con este o hasta que se haga sys_exit sobre el proceso.

}

int sys_pthread_join(int TID, int *retval)
{
	if(TID == current()->TID)
		return -EDEADLK; // isma: joining itself
  
	struct list_head *pos;
	struct task_struct *t_thread_to_join_with = NULL;
	list_for_each(pos, current()->threads_process){ // isma: threads_process is a reference to the sentinel of the threads queue of a certain process
		struct task_struct *t_aux = list_head_to_task_struct(pos);
		if(TID == t_aux->TID){
			t_thread_to_join_with = t_aux;
			break;
		}
	}

	if(t_thread_to_join_with == NULL)
		return -ESRCH;

	if(current()->joined == t_thread_to_join_with) // isma: if I'm trying to join the same thread that joined me
		return -EDEADLK;
 

	if(t_thread_to_join_with->joined != NULL) // isma: the thread i'm trying to join with was already joined by another thread
		return -EINVAL;

	if(t_thread_to_join_with->state != ST_ZOMBIE){ // isma: the thread i'm trying trying to join hasn't finished its execution yet.
		t_thread_to_join_with->joined = current(); //isma: that thread is officialy joined by me
		current()->state = ST_BLOCKED;
		list_add_tail(&(current()->list), &blockedqueue); // isma: we block until that thread finishes its execution
		switch(sched_next_decide_level()){ 
			case 2: 
				//isma: Todos los demás threads del proceso están bloqueados. El unico caso donde se me ocurre que puede ocurrir esto sin que sea un desastre absoluto es cuando estan haciendo cosas sobre un mutex junto a OTRO proceso, si no nada les podría desbloquear.
				sched_next_rr_level2(); 
				break;
			case 1: 
				sched_next_rr_level1();
				break;
			default: 
				panic("ERROR pthread_join: Nos hemos quedado en un sistema en el que todo está bloqueado");
				sched_next_rr(idle_task);
				break;
		}
	}

	// isma: this point will be reached when the thread_to_join_with finish its execution (exits).

	if(retval != NULL)
		*retval = t_thread_to_join_with->retval;

	// isma: Liberamos recursos del thread_to_join_with (que ha acabado ejecucion):

	int user_stack_VPN = calc_VPN_user_stack(TID);
	page_table_entry *process_PT = get_PT(t_thread_to_join_with); // isma: dado que son threads del mismo proceso, esta PT es la misma que la PT de current()
	free_frame(get_frame(process_PT, user_stack_VPN));
    	del_ss_pag(process_PT, user_stack_VPN);

	t_thread_to_join_with->TID = -1; // isma: no es necesario
	list_del(&(t_thread_to_join_with->list_threads)); // isma: lo quitamos de la cola de threads del proceso
	list_add_tail(&(t_thread_to_join_with->list), &freequeue);
		
	return 0;
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
