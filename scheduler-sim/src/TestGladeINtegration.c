#include <gtk/gtk.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtkx.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <stdio.h>


//For compiling
//gcc -Wno-format -o TestUI TestGladeINtegration.c -Wno-deprecated-declarations -Wno-format-security -lm `pkg-config --cflags --libs gtk+-3.0` -export-dynamic

GtkWidget     *window;
GtkWidget     *Gtk_Fixed;
GtkWidget     *progress_bar1;
GtkWidget     *progress_bar2;
GtkWidget     *button;
GtkWidget     *label;
GtkBuilder    *builder;

int countDownMax, countCurrent;

gboolean timer_handler(GtkWidget *);

int main(int argc, char *argv[]) {
  gtk_init(&argc, &argv);

//---------------------------------
// XML COnnection
// ---------------------------------

  builder = gtk_builder_new_from_file ("InterfaceGlade.glade");

  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  gtk_builder_connect_signals(builder, NULL);

  Gtk_Fixed = GTK_WIDGET(gtk_builder_get_object(builder, "GtkFixed"));
  progress_bar1 = GTK_WIDGET(gtk_builder_get_object(builder, "process1"));
  progress_bar2 = GTK_WIDGET(gtk_builder_get_object(builder, "process2"));
  label = GTK_WIDGET(gtk_builder_get_object(builder, "label"));

  button = GTK_WIDGET(gtk_builder_get_object(builder, "button"));

  gtk_widget_show (window);

  countCurrent = countDownMax = 0;

  gtk_main();

  return EXIT_SUCCESS;
}

gdouble fraction = 0.0;
gdouble test = 0.0;
gchar *display;
//gdouble fraction2 = 0.0;

/*static gboolean
fill (gpointer user_data)
{
  GtkWidget *progress_bar1 = user_data;
  //GtkWidget *progress_bar2 = user_data2;

  gdouble fraction = gtk_progress_bar_get_fraction (GTK_PROGRESS_BAR (progress_bar1));

  fraction += 0.1;
  if (fraction < 1.0){
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar1), fraction);
    return TRUE;
  }
  if (fraction >= 1.0){
    fraction = 0.0;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar1), fraction);
  }
  return FALSE;
}

static gboolean
fill2 (gpointer user_data)
{
  GtkWidget *progress_bar2 = user_data;
  //GtkWidget *progress_bar2 = user_data2;

  gdouble fraction2 = gtk_progress_bar_get_fraction (GTK_PROGRESS_BAR (progress_bar2));

  fraction2 += 0.1;
  if (fraction2 < 1.0){
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar2), fraction2);
    return TRUE;
  }
  if (fraction2 >= 1.0){
    fraction2 = 0.0;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar2), fraction2);
  }
  return FALSE;
}*/



const int workload = 1;

static gboolean fill (gpointer user_data){
  GtkWidget *progress_bar1 = user_data;
  char tmp[20]="6";

  gdouble fraction = gtk_progress_bar_get_fraction (GTK_PROGRESS_BAR (progress_bar1));

  //gtk_label_set_text (GTK_LABEL(label), (const gchar* ) tmp); // indicator***********

  int pid = 0;
  for(pid=0;pid<2;pid++){
    //itoa(pid,tmp,4);
    //gtk_label_set_text (GTK_LABEL(label), (const gchar* ) tmp); // indicator***********
    int end = workload*50;
    int indx_term=1;
    long double coeff = 2.0;
    long double result=0.0;
    for(int i=indx_term; i <= end; i++){
      //gtk_label_set_text (GTK_LABEL(label), (const gchar* ) tmp); // indicator***********
      if(i%2==1){
        result+=coeff/i;
        coeff*=i;
        test = 1.0/end;
        if (fraction < 1.0){
          display = g_strdup_printf("%d", i);
          gtk_label_set_text (GTK_LABEL(label), display);
          gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar1), test*i);
          return TRUE;
        }
        if (fraction >= 1.0){
          fraction = 0.0;

          gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar1), fraction);
        }
        return FALSE;
      }
      else {
        display = g_strdup_printf("%d", 7);
        gtk_label_set_text (GTK_LABEL(label), display);
        coeff/=i;
      }

      //printf("pid: %d, terms: %d, approx: %0.10Lf\n", pid,i,result);

    }


  }
  return 0;
}

//       ************ Triggers ***************
void onButton (GtkButton *b){
  g_timeout_add(1, fill, GTK_PROGRESS_BAR (progress_bar1));
  //g_timeout_add(800, fill2, GTK_PROGRESS_BAR (progress_bar2));
}
