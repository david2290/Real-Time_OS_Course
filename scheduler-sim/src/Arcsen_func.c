#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <gmodule.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <setjmp.h>

#define NUM_PROCESSES 5

typedef sigjmp_buf Env_buf;

typedef struct Process{
  Env_buf env;
  int pid;
  int workload;
  int tickets;
  int quantum;
  int arrival_time;
  bool finished;
  long double coeff;
  long double result;
  int indx_term;
}Process;


typedef struct Lottery_task{
  GList *lst;
  int quantum;
  int total_tickets;
}Lottery_task;

static bool timeout = false;

void sig_timeout(int signo){timeout=true;}

void config_timeout(){signal(SIGALRM,sig_timeout);}

void start_timeout_timer(int quantum){ualarm(quantum,0);}

double pi_approx_arcsen(Process *proc, Env_buf *scheduler_env){
  long double result = proc->result;
  long double coeff = proc->coeff;
  int end = proc->workload*50;
  for(int i=proc->indx_term; i <= end; i++){
    if(i%2==1){
      result+=coeff/i;
      coeff*=i;
    }else
      coeff/=i;
/*
Suspend
*/
  if(i%500==0)
    printf("pid:%d, terms:%d ,approx:%0.10Lf\n",proc->pid,i,result);
  if(timeout){
    proc->indx_term=i+1;
    proc->result=result;
    proc->coeff=coeff;
    siglongjmp(*scheduler_env,0);
  }
/*
Suspend
*/
  }
  proc->finished=true;
  return proc->result;
}

Process *
choose_winner(Lottery_task *lt){
  Process *p;
  int random_ticket = rand()%lt->total_tickets;
  int acc_tickets = 0;
  GList *l = lt->lst;
  p = l->data;
  while(acc_tickets+p->tickets<=random_ticket && l != NULL){
    acc_tickets+=p->tickets;
    l = l->next;
    p = l->data;
  }
  return p;
}

void
update_arrival(
    Lottery_task *not_ready_queue,
    Lottery_task *ready_queue)
{
  static time_t init_time = 0;
  if(init_time==0) init_time = time(NULL);
  time_t local_time = time(NULL);
  for(GList *l=not_ready_queue->lst; l!=NULL; l=l->next){
    Process *p=l->data;
    if(p->arrival_time <= (local_time-init_time)){
      ready_queue->total_tickets += p->tickets;
      not_ready_queue->total_tickets -= p->tickets;
      ready_queue->lst = g_list_append(ready_queue->lst,(gpointer)p);
      not_ready_queue->lst = g_list_remove(not_ready_queue->lst,(gpointer)p);
    }
  }
}

void
update_process_finished(
    Lottery_task *ready_queue,
    Lottery_task *finished_queue)
{
  for(GList *l=ready_queue->lst; l!=NULL; l=l->next){
    Process *p=l->data;
      if(p->finished){
        ready_queue->total_tickets -= p->tickets;
        finished_queue->total_tickets += p->tickets;
        finished_queue->lst = g_list_append(finished_queue->lst,(gpointer)p);
        ready_queue->lst = g_list_remove(ready_queue->lst,(gpointer)p);
      }
    }
}


int lottery_scheduler(Lottery_task *lt){
  int total_processes = g_list_length(lt->lst);
  Lottery_task not_ready_queue = *lt;
  Lottery_task ready_queue = {.lst=NULL, .quantum=lt->quantum, .total_tickets=0};
  Lottery_task finished_queue = {.lst=NULL, .quantum=lt->quantum, .total_tickets=0};
  config_timeout();
  while(g_list_length(finished_queue.lst)<total_processes){
    Env_buf scheduler_env;
    sigsetjmp(scheduler_env,1);
    update_arrival(&not_ready_queue,&ready_queue);
    if(ready_queue.lst==NULL) continue;
    Process *pid_winner = choose_winner(&ready_queue);
    if(pid_winner!=NULL){
      start_timeout_timer(lt->quantum);
      pi_approx_arcsen(pid_winner,&scheduler_env);
    }else continue;
    update_process_finished(&ready_queue,&finished_queue);
  }
  return 0;
}

Lottery_task
create_lottery_task(int number){
  Lottery_task lt;
  lt.lst=NULL;
  lt.quantum=100;
  srand(time(0));
  lt.total_tickets=0;
  for(int i=0;i<NUM_PROCESSES;i++){
    Process *p = g_new(Process,1);
    p->pid=i;
    p->workload=100;
    p->tickets=100;
    p->quantum=100;
    p->arrival_time=1;
    p->coeff=2.0;
    p->result=0.0;
    p->finished=false;
    p->indx_term=1;
    lt.lst=g_list_append(lt.lst,(gpointer) p);
    lt.total_tickets+=p->tickets;
  }
  return lt;
}

void free_lottery_task(Lottery_task* lt){
  g_list_free_full(lt->lst,g_free);
}

int main(){
  Lottery_task lt = create_lottery_task(NUM_PROCESSES);
  lottery_scheduler(&lt);
  free_lottery_task(&lt);
  return 0;
}
// gcc  Arcsen_func.c -o ArcSenProgram -lm -fsanitize=address -fno-omit-frame-pointer `pkg-config --cflags --libs glib-2.0` -g -Wall
