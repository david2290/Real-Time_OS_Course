#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <setjmp.h>


typedef sigjmp_buf Env_buf;

typedef struct Process{
  Env_buf env;
  int pid;
  int workload;
  int tickets;
  int quantum;
  bool suspended;
  bool finished;
}Process;

double pi_approx_arcsen(Process *proc, Env_buf *scheduler_env){
  long double coeff=1.0;
  long double result=1.0;
  int end=proc->workload*50;
  int suspend=proc->quantum*50;
  for(int i=1; i <= end; i++){
    if(i%2==1){
      result+=coeff/i;
      coeff*=i;
    }else
      coeff/=i;
/*
Suspend
*/
    if(i%suspend!=0){
      setjmp(proc->env);
   }else{
      proc->suspended=true;
      printf("jmp: %d\n",i);
      longjmp(*scheduler_env,0);
    }
    if(i%500==0)
      printf("pid:%d, terms:%d ,approx:%0.10Lf\n",proc->pid,i,result);
/*
Suspend
*/
  }
  proc->finished=true;

  return result;
}

int choose_winner(Process *p,int size){
  int winner=0;
  for(int i=0;i<size;i++){
    if(!p[i].finished){
     winner = p[i].pid;
     printf("winner: %d\n", winner);
    }
  }
  return winner;
}
bool processes_are_finished(Process *p,int size){
  bool allfinished=false;
  for(int i=0;i<size;i++)
    allfinished = allfinished && p[i].finished;
  return allfinished;
}
int scheduler(Process *p, int size){
  Env_buf scheduler_env;
  setjmp(scheduler_env);
  int pid_winner = choose_winner(p,2);
  printf("winner: pid%d\n",pid_winner);
  if(!p[pid_winner].suspended){
    pi_approx_arcsen(&p[pid_winner],&scheduler_env);
    printf("finished: %d\n",p[pid_winner].finished);
  }else
    longjmp(p[pid_winner].env,0);
  if(!processes_are_finished(p,2)){
    longjmp(scheduler_env,0);
  }
  return 0;
}

int main(){
  Process p[2];
  for(int i=0;i<2;i++){
    p[i].pid=i;
    p[i].workload=10;
    p[i].tickets=1;
    p[i].suspended=false;
    p[i].quantum=100;
    p[i].finished=false;
  }
  scheduler(p,1);
  return 0;
}
// gcc Arcsen_func.c -o ArcSenProgram -lm
