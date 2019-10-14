#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdlib.h>
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

int choose_winner(Process *p, int size, int total_tickets){
  int random_ticket = rand()%total_tickets;
  int acc_tickets = 0;
  for(int i=0;i<size;i++){
    if(acc_tickets+p[i].tickets>random_ticket){
      if(!p[i].finished){
        return p[i].pid;
      }else{
        random_ticket = rand()%total_tickets;
        i=-1;
        acc_tickets = 0;
        continue;
      }
    }
    acc_tickets+=p[i].tickets;
  }
}
bool processes_are_finished(Process *p,int size){
  bool allfinished=true;
  for(int i=0;i<size;i++)
    allfinished = allfinished && p[i].finished;
  return allfinished;
}
int lottery_scheduler(Process *p, int size, int total_tickets){
  Env_buf scheduler_env;
  setjmp(scheduler_env);
  int pid_winner = choose_winner(p,size,total_tickets);
  //printf("winner: pid%d\n",pid_winner);
  pi_approx_arcsen(&p[pid_winner],&scheduler_env);
  if(!processes_are_finished(p,size)){
    longjmp(scheduler_env,0);
  }
  return 0;
}


int main(){
  Process *p=(Process*)malloc(NUM_PROCESSES*sizeof(Process));
  srand(time(0));
  int total_tickets=0;
  for(int i=0;i<NUM_PROCESSES;i++){
    p[i].pid=i;
    p[i].workload=1000;
    p[i].tickets=100;
    p[i].suspended=false;
    p[i].quantum=1;
    p[i].coeff=2.0;
    p[i].result=0;
    p[i].finished=false;
    p[i].indx_term=1;
    total_tickets+=p[i].tickets;
  }
  lottery_scheduler(p,NUM_PROCESSES,total_tickets);
  free(p);
  return 0;
}
// gcc Arcsen_func.c -o ArcSenProgram -lm
