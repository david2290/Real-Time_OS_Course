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

#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>

#define NUM_PROCESSES 5  // CAmbiar por los procesos q estan en el archivo
//***************** GTK ***************************
//gcc -Wno-format -o TestUI TestGladeINtegration.c -Wno-deprecated-declarations -Wno-format-security -lm `pkg-config --cflags --libs gtk+-3.0` -export-dynamic

GtkWidget     *window;
GtkWidget     *Gtk_Fixed;
GtkWidget     *progress_bar1;
GtkWidget     *progress_barArreglo[25]; // OJO
GtkWidget     *progress_bar2;
GtkWidget     *button;
GtkWidget     *label;
GtkWidget     *LabelArreglo[25];        // OJO

GtkBuilder    *builder;




typedef sigjmp_buf Env_buf;   // para guardar el contexto

typedef struct Process{   //
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

void sig_timeout(int signo){timeout=true;} //CUando pasa la interrupcion

void config_timeout(){signal(SIGALRM,sig_timeout);}

void start_timeout_timer(int quantum){ualarm(quantum,0);}

void start_timeout_timer(int quantum){
  timeout=false;
  ualarm(quantum,0);
}

gdouble fraction = 0.0;
gdouble test = 0.0;
gchar *display;

double pi_approx_arcsen(Process *proc, Env_buf *scheduler_env){
  char tmp[20] = "6";// BORRAR DESPUES
  gdouble fraction = gtk_progress_bar_get_fraction (GTK_PROGRESS_BAR (progress_bar1));


  long double result = proc->result;
  long double coeff = proc->coeff;
  int end = proc->workload*50;

  for(int i=proc->indx_term; i <= end; i++){
    if(i%2==1){
      result+=coeff/i;
      coeff*=i;
      //test = 1.0/end;
      //display = g_strdup_printf("%d", i);
      //gtk_label_set_text (GTK_LABEL(label), display);
      //gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar1), test*i);

    }else
      coeff/=i;
/*
Suspend
*/
  if(i%500==0)//cada 500 terminos hace un print
    //printf("pid:%d, terms:%d ,approx:%0.10Lf\n",proc->pid,i,result); //Dif las barras
    test = 1.0/end;
    display = g_strdup_printf("%d", i);
    gtk_label_set_text (GTK_LABEL(label), display);
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar1), test*i);
  // Se podria agregar q actualice la barra

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
    Lottery_task *not_ready_queue,                   // Preguntar a kaleb***********
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
  Lottery_task not_ready_queue = *lt;                                                //
  Lottery_task ready_queue = {.lst=NULL, .quantum=lt->quantum, .total_tickets=0};    // se definen los estados de los procesos
  Lottery_task finished_queue = {.lst=NULL, .quantum=lt->quantum, .total_tickets=0}; //
  config_timeout();
  while(g_list_length(finished_queue.lst)<total_processes){   //mientras no haya terminado con todos los procesos
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
create_lottery_task(int number){ //pasa el archivo
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
    lt.lst=g_list_append(lt.lst,(gpointer) p); // apend de una lista de procesos
    lt.total_tickets+=p->tickets;
  }
  return lt;
}

void free_lottery_task(Lottery_task* lt){
  g_list_free_full(lt->lst,g_free);
}



// tRIGGER PARA INICIAR LOS PROCESOS
void onButton (GtkButton *b){
  Lottery_task lt = create_lottery_task(NUM_PROCESSES);
  lottery_scheduler(&lt);
  free_lottery_task(&lt);
  return 0;
}

int main(int argc, char *argv[]) {
  gtk_init(&argc, &argv);
//---------------------------------
// XML COnnection
// ---------------------------------
  builder = gtk_builder_new_from_file ("InterfaceGlade.glade");
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_builder_connect_signals(builder, NULL);

// Hacer un arreglo progress_bars tomando en cuenta los labels
  Gtk_Fixed = GTK_WIDGET(gtk_builder_get_object(builder, "GtkFixed"));
  progress_bar1 = GTK_WIDGET(gtk_builder_get_object(builder, "process1"));
  progress_bar2 = GTK_WIDGET(gtk_builder_get_object(builder, "process2"));
  label = GTK_WIDGET(gtk_builder_get_object(builder, "label"));

  button = GTK_WIDGET(gtk_builder_get_object(builder, "button"));
  gtk_widget_show (window);
  gtk_main();

  return EXIT_SUCCESS;
}





// gcc  Arcsen_func.c -o ArcSenProgram -lm -fsanitize=address -fno-omit-frame-pointer `pkg-config --cflags --libs glib-2.0` -g -Wall
