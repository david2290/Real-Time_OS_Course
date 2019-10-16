#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <gmodule.h>
#include <time.h>
#include <setjmp.h>

#define NUM_PROCESSES 5

typedef sigjmp_buf Env_buf;

typedef struct Process{
  Env_buf env;
  int pid;
  int workload;
  int tickets;
  int quantum;
  bool suspended;
  bool finished;
  long double coeff;
  long double result;
  int indx_term;
}Process;


typedef struct Lottery_task{
  GList *lst;
  int total_tickets;
}Lottery_task;


double pi_approx_arcsen(Process *proc, Env_buf *scheduler_env){
  long double result = proc->result;
  long double coeff = proc->coeff;
  int end = proc->workload*50;
  int suspend = proc->quantum*50;
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
  if(i%suspend==0 && i>0){
    proc->indx_term=i+1;
    proc->result=result;
    proc->coeff=coeff;
    proc->suspended=true;
    longjmp(*scheduler_env,0);
  }
/*
Suspend
*/
  }
  proc->suspended=false;
  proc->finished=true;
  return proc->result;
}

Process *
choose_winner(Lottery_task *lt){
  Process *p;
  do{
    int random_ticket = rand()%lt->total_tickets;
    int acc_tickets = 0;
    GList *l = lt->lst;
    p = l->data;
    while(acc_tickets+p->tickets<=random_ticket && l != NULL){
      acc_tickets+=p->tickets;
      l = l->next;
      p = l->data;
    }
  }while(p->finished);
  return p;
}


bool processes_are_finished(Lottery_task *lt){
  bool allfinished=true;
  for(GList *l = lt->lst; l!=NULL; l=l->next){
    Process *p = l->data;
    allfinished = allfinished && p->finished;
  }
  return allfinished;
}
int lottery_scheduler(Lottery_task *lt){
  Env_buf scheduler_env;
  setjmp(scheduler_env);
  Process *pid_winner = choose_winner(lt);
  //printf("winner: pid%d\n",pid_winner);
  pi_approx_arcsen(pid_winner,&scheduler_env);
  if(!processes_are_finished(lt)){
    longjmp(scheduler_env,0);
  }
  return 0;
}

Lottery_task
create_lottery_task(int number){
  Lottery_task lt;
  lt.lst=NULL;
  srand(time(0));
  lt.total_tickets=0;
  for(int i=0;i<NUM_PROCESSES;i++){
    Process *p = g_new(Process,1);
    p->pid=i;
    p->workload=100;
    p->tickets=100;
    p->suspended=false;
    p->quantum=1;
    p->coeff=2.0;
    p->result=0;
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
