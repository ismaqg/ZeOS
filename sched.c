/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <types.h>
#include <hardware.h>
#include <segment.h>
#include <sched.h>
#include <mm.h>
#include <io.h>
#include <utils.h>
#include <p_stats.h>

// Address of errno (never changes)
#define perrno (int *)0x109000

/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS + 2]
    __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

#if 0
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
#endif


extern struct list_head blocked;

// Blocked queue
struct list_head blockedqueue;
// Free task structs
struct list_head freequeue;
// Ready queue
struct list_head readyqueue;

void init_stats(struct stats *s)
{
  s->user_ticks = 0;
  s->system_ticks = 0;
  s->blocked_ticks = 0;
  s->ready_ticks = 0;
  s->elapsed_total_ticks = get_ticks();
  s->total_trans = 0;
  s->remaining_ticks = get_ticks();
}

/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry *get_DIR(struct task_struct *t)
{
  return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry *get_PT(struct task_struct *t)
{
  return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr)) << 12);
}

int allocate_DIR(struct task_struct *t)
{
  int pos;

  pos = ((int)t - (int)task) / sizeof(union task_union);

  t->dir_pages_baseAddr = (page_table_entry *)&dir_pages[pos];

  return 1;
}

void cpu_idle(void)
{
  __asm__ __volatile__("sti"
                       :
                       :
                       : "memory");

  while (1)
  {
    ;
  }
}


int remaining_quantum_process = 0;
int remaining_quantum_thread = 0;

int get_quantum_process(struct task_struct *t)
{
  return t->total_quantum;
}

int get_quantum_thread(struct task_struct *t)
{
  return t->quantum_thread;
}

void set_quantum(struct task_struct *t, int new_quantum)
{
  t->total_quantum = new_quantum;
}

struct task_struct *idle_task = NULL;

void update_sched_data_rr(void)
{
  if(--remaining_quantum_process < 0) remaining_quantum_process = 0; // isma: Evitar que siga decrementando infinitamente hasta underflow
  if(--remaining_quantum_thread < 0) remaining_quantum_thread = 0;
}

//0: switch not needed. 1: se necesita planificador de 1r nivel (thread mismo proceso). 2: planificador 2o nivel (thread distinto proceso)
int needs_sched_rr(void) // isma: Solo se llamara en tick de reloj
{
   if(list_empty(&readyqueue))
	return 0;

   // si llegamos a este punto: ready NO vacía.

   if (current() == idle_task) 
	return 2;

   //si llegamos a este punto: ready NO vacia y en RUN NO idle_task

   if(remaining_quantum_process > 0 && remaining_quantum_thread > 0)
	return 0;

   //si llegamos aqui: ready NO vacia, en RUN NO idle_task y algún quantum está finalizado

   struct list_head *pos; 
   struct task_struct *t_thread_same_process = NULL;
   struct task_struct *t_thread_different_process = NULL;
   list_for_each(pos, &readyqueue){
	 struct task_struct *t = list_head_to_task_struct(pos);
	 if(t_thread_same_process == NULL && t->PID == current()->PID) t_thread_same_process = t;
	 else if(t_thread_different_process == NULL && t->PID != current()->PID) t_thread_different_process = t;
   }

   if(remaining_quantum_thread <= 0 && remaining_quantum_process > 0){
	if(t_thread_same_process != NULL)
		return 1; 
	return 0;
   }

   //si llegamos aqui: ready NO vacia, en RUN NO idle_task y quantum proceso finalizado (<=0) SEGURO (y el de thread no se sabe)

   if(t_thread_different_process != NULL) return 2; 
	
   //si llegamos aqui, como ready NO vacia, es que sé al 100% que en ready hay otros threads de mi mismo proceso (pero no de otro)

   if(remaining_quantum_thread <= 0) //isma: Ahora sí que nos interesa saber si aparte de quantumprocess <= 0 tambien teniamos thread<=0
	return 1;
   return 0;	
   
   
}

void update_process_state_rr(struct task_struct *t, struct list_head *dst_queue)
{
  if (t->state != ST_RUN)
    list_del(&(t->list));
  if (dst_queue != NULL)
  {
    list_add_tail(&(t->list), dst_queue);
    if (dst_queue != &readyqueue)
      t->state = ST_BLOCKED;
    else
    {
      update_stats(&(t->p_stats.system_ticks), &(t->p_stats.elapsed_total_ticks));
      t->state = ST_READY;
    }
  }
  else
    t->state = ST_RUN;
}

// Simlemente pone t en RUN
void sched_next_rr(struct task_struct *t) 
{
	t->state = ST_RUN;
  	remaining_quantum_process = get_quantum_process(t);
	remaining_quantum_thread = get_quantum_thread(t);

  	update_stats(&(current()->p_stats.system_ticks), &(current()->p_stats.elapsed_total_ticks));
  	update_stats(&(t->p_stats.ready_ticks), &(t->p_stats.elapsed_total_ticks));
  	t->p_stats.total_trans++;

  	task_switch((union task_union *)t);
}

//planificador de 1r nivel (thread mismo proceso)
void sched_next_rr_level1(void) // A esta funcion solo se la llamara cuando tengamos seguro al 100% que en ready quedan threads DEL MISMO proceso
{
	//solo entrara en esta funcion si readyqueue NO vacia

	struct task_struct *t = NULL; // isma: t es el nuevo que entra a run	
	struct list_head *pos; 
	list_for_each(pos, &readyqueue){
		 struct task_struct *tmp = list_head_to_task_struct(pos);
		 if(tmp->PID == current()->PID){
			t = tmp;
			break;
		 } 
	}


	if(t == NULL) println("ERROR: se ha ejecutado sched_level2 con solo threads de otros procesos en ready");
        list_del(&(t->list));

        sched_next_rr(t);

}

//planificador 2o nivel (thread distinto proceso)
void sched_next_rr_level2(void) // A esta funcion solo se la llamara cuando tengamos seguro al 100% que en ready hay otro proceso DISTINTO
{
	//solo entrara en esta funcion si readyqueue NO vacia

	struct task_struct *t = NULL; // isma: t es el nuevo que entra a run	
	struct list_head *pos; 
	list_for_each(pos, &readyqueue){
		 struct task_struct *tmp = list_head_to_task_struct(pos);
		 if(tmp->PID != current()->PID){
			t = tmp;
			break;
		 } 
	}


	if(t == NULL) println("ERROR: se ha ejecutado sched_level2 con solo threads del mismo procesos en ready");
        list_del(&(t->list));

        sched_next_rr(t);
}

//0: Si no readyqueue empty. 1: Si hay que llamar al de 1r nivel (otro thread del proceso). 2: Si hay que llamar al 2o nivel (otro proceso)
int sched_next_decide_level(void){ // isma: Solo se llama a esta funcion en caso de tener que forzar un task_switch (es decir, forzar que el de RUN salga de ahi).
// alex: Como no se la llamara a partir de una interrupcion de reloj, sabemos que aqui aun queda quantum. Pero igualmente hay que forzar el task_switch 
	
	if(list_empty(&readyqueue))
		return 0;

	//si llega aqui: hay alguien en ready

	struct task_struct *t = NULL; 
	struct list_head *pos; 
	list_for_each(pos, &readyqueue){
		 struct task_struct *tmp = list_head_to_task_struct(pos);
		 if(tmp->PID == current()->PID){
			t = tmp;
			break;
		 } 
	}
	if(t == NULL) // nadie de los de ready es otro thread del mismo proceso de quien está en run y va a salir.
		return 2;
	else
		return 1;
}

void schedule()
{
  update_sched_data_rr();
  int level;
  if ((level = needs_sched_rr())) // Si no retorna 0, entra.
  {
    update_process_state_rr(current(), &readyqueue);
    if(level == 1) sched_next_rr_level1();
    else if(level == 2) sched_next_rr_level2();
  }
}

void init_idle(void)
{
  struct list_head *l = list_first(&freequeue);
  list_del(l);
  struct task_struct *c = list_head_to_task_struct(l);
  union task_union *uc = (union task_union *)c;

  c->PID = 0;

  c->total_quantum = DEFAULT_QUANTUM_PROCESS;

  init_stats(&c->p_stats);

  allocate_DIR(c);

  uc->stack[KERNEL_STACK_SIZE - 1] = (unsigned long)&cpu_idle; /* Return address */
  uc->stack[KERNEL_STACK_SIZE - 2] = 0;                        /* register ebp */

  c->register_esp = (int)&(uc->stack[KERNEL_STACK_SIZE - 2]); /* top of the stack */

  idle_task = c;
}

void setMSR(unsigned long msr_number, unsigned long high, unsigned long low);

void init_task1(void)
{
  struct list_head *l = list_first(&freequeue);
  list_del(l);
  struct task_struct *c = list_head_to_task_struct(l);
  union task_union *uc = (union task_union *)c;

  c->PID = 1;

  c->total_quantum = DEFAULT_QUANTUM_PROCESS;

  c->state = ST_RUN;

  remaining_quantum_process = c->total_quantum;

  init_stats(&c->p_stats);

  allocate_DIR(c);

  set_user_pages(c);

  tss.esp0 = (DWord) & (uc->stack[KERNEL_STACK_SIZE]);
  setMSR(0x175, 0, (unsigned long)&(uc->stack[KERNEL_STACK_SIZE]));

  set_cr3(c->dir_pages_baseAddr);
}

void init_freequeue()
{
  int i;

  INIT_LIST_HEAD(&freequeue);

  /* Insert all task structs in the freequeue */
  for (i = 0; i < NR_TASKS; i++)
  {
    task[i].task.PID = -1;
    list_add_tail(&(task[i].task.list), &freequeue);
  }
}

void init_sched()
{
  init_freequeue();
  INIT_LIST_HEAD(&readyqueue);
  INIT_LIST_HEAD(&blockedqueue);
}

struct task_struct *current()
{
  int ret_value;

  return (struct task_struct *)(((unsigned int)&ret_value) & 0xfffff000);
}

struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return (struct task_struct *)((int)l & 0xfffff000);
}

/* Do the magic of a task switch */
void inner_task_switch(union task_union *new)
{
  page_table_entry *new_DIR = get_DIR(&new->task);

  /* Update TSS and MSR to make it point to the new stack */
  tss.esp0 = (int)&(new->stack[KERNEL_STACK_SIZE]);
  setMSR(0x175, 0, (unsigned long)&(new->stack[KERNEL_STACK_SIZE]));

  //TODO: SACAR DE AQUÍ EL int *perrno = (int*)0x109000. EL PROFE HA HABLADO DE HACER UN DEFINE

  //int *perrno = (int*)0x109000; // isma: Address of errno (never changes)
  current()->errno = *perrno;

  /* TLB flush. New address space */
  set_cr3(new_DIR);

  *perrno = new->task.errno;

  switch_stack(&current()->register_esp, new->task.register_esp);
}

/* Force a task switch assuming that the scheduler does not work with priorities */
void force_task_switch()
{
  update_process_state_rr(current(), &readyqueue);

  switch(sched_next_decide_level()){ 
	case 2:
		sched_next_rr_level2();
		break;
	case 1:
		sched_next_rr_level1();
		break;
	default: // empty ready_queue
		sched_next_rr(idle_task);
		break;
  }

}
