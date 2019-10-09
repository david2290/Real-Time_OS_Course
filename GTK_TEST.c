/*#include <gtk/gtk.h>
#include <glib.h>

static float percent = 0.0;

static gboolean inc_progress(gpointer data)
{
  percent+=0.05;
  if(percent > 1.0){
    percent = 0.0;
  }
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(data),percent);
  char c[3];
  sprintf(c,"%d",static_cast<int>(percent*100));
  gtk_progress_bar_set_text(GTK_PROGRESS_BAR(data),c);
}

int main(int argc, char* argv[])
{
  gtk_init(&argc, &argv);
  GtkWidget *window, *progress;
  window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_signal_connect(window, "delete-event",G_CALLBACK (gtk_main_quit), NULL);

  progress = gtk_progress_bar_new();
  //gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress), 0.5);
  //gtk_progress_bar_set_text(GTK_PROGRESS_BAR(progress), "50%");
//  gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(progress), GTK_PROGRESS_TOP_TO_BOTTOM);
  g_timeout_add(250, inc_progress,progress);
  gtk_container_add(GTK_CONTAINER (window), progress);

  gtk_widget_show_all(window);
  gtk_main();
  return 0;

}*/

#include <gtk/gtk.h>

static gboolean fill (gpointer user_data)
{
  GtkWidget *progress_bar = user_data;

  gdouble fraction;
  fraction = gtk_progress_bar_get_fraction (GTK_PROGRESS_BAR (progress_bar));

  fraction += 0.1;

  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar), fraction);

  if (fraction < 1.0)
    return TRUE;
  return FALSE;
}

static void activate (GtkApplication *app, gpointer user_data){
  GtkWidget *window;
  GtkWidget *progress_bar;

  gdouble fraction = 0.0;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "ProgressBar Example");
  gtk_window_set_default_size (GTK_WINDOW (window), 220, 20);

  progress_bar = gtk_progress_bar_new();
  gtk_container_add (GTK_CONTAINER (window), progress_bar);

  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progress_bar), fraction);

  g_timeout_add(500, fill, GTK_PROGRESS_BAR (progress_bar));

  gtk_widget_show_all (window);

}


int main (int argc, char **argv){
  GtkApplication *app;
  int status;

  app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
