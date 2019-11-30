#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "schedule-utils.h"
#include "latex_tools.h"
//
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <ctype.h>

#define MAX_TASKS 6

//***************** GTK ***************************


GtkWidget     *window;
GtkWidget     *Gtk_Fixed;
GtkWidget     *period_array_widget_ptr[MAX_TASKS];
GtkWidget     *compute_array_widget_ptr[MAX_TASKS];
GtkWidget     *label_array_widget_ptr[MAX_TASKS];
GtkWidget     *button1;
GtkWidget     *SwEnable_RM;
GtkWidget     *SwEnable_EDF;
GtkWidget     *SwEnable_LLF;
GtkWidget     *SwDisplay1;
GtkWidget     *TotalTask;
GtkWidget     *Enter_button;
GtkBuilder    *builder;

int totalTask = 0;

//gboolean Display = gtk_switch_get_active(GTK_SWITCH(SwDisplay1));

void print_trace(SC_SimTrace *struct_trace_ptr){
	int len = struct_trace_ptr->trace->len;
	if(struct_trace_ptr->utilization <= 1){
		for(int i=0; i<len ; i++){
			int running_process_index = g_array_index(struct_trace_ptr->trace,int,i);
			if(running_process_index != -1){
				printf("\n(%d-%d)| t%d",i,i+1, running_process_index);
			}
			else{
				printf("\n(%d-%d)",i,i+1);
			}
		}printf("\n");
		if(struct_trace_ptr->error){
			int incomplete_task_id = struct_trace_ptr->incomplete_task_id;
			printf("\nthe system is not schedulable because the task %d cant finish its job in its specified period\n",incomplete_task_id);
		}
	}else{
		printf("\nthis system isn't schedulable because\nthe sum of Ei/Pi = %f > 1\n",struct_trace_ptr->utilization);
	}
}

void plot_array_trace_pdf(GArray *struct_trace_ptr,gboolean Display_format){
	FILE* file = RT_create_file_buffer();
	printf("step 0\n");
	if(Display_format){
		RT_print_trace_mixed(struct_trace_ptr,file);
	}else{
		RT_print_trace(struct_trace_ptr,file);
	}
	fclose(file);
	RT_export_pdf();
	RT_open_window();
}

void run_simulation(SC_SimTrace *struct_trace_ptr, SC_Policy schedulable_policy){
	GArray *readyQueue = g_array_new (FALSE, FALSE, sizeof(SC_Process));
    SC_createSystemFromFile(readyQueue);
	struct_trace_ptr->number_of_tasks = readyQueue->len;
	float utilization = SC_calc_cpu_utilization(readyQueue);
	struct_trace_ptr->utilization=utilization;
	if(utilization>1){
		struct_trace_ptr->error=true;
	}else{
		SC_schedule(readyQueue,schedulable_policy,struct_trace_ptr);
	}
	g_array_free(readyQueue, TRUE);
}

SC_Policy assign_policy(int policy_id){
	switch (policy_id) {
		case SC_RM_ID:
			return SC_RM_policy;
		break;
		case SC_EDF_ID:
			return SC_EDF_policy;
		break;
		case SC_LLS_ID:
			return SC_LLS_policy;
		break;
		default: break;
	}
	return SC_RM_policy;
}

void on_Enter_clicked (GtkButton *c){
	totalTask = gtk_spin_button_get_value (GTK_SPIN_BUTTON(TotalTask));
	for(int i=0;i<totalTask; i++){
		gtk_widget_show(label_array_widget_ptr[i]);
		gtk_widget_show(compute_array_widget_ptr[i]);
		gtk_widget_show(period_array_widget_ptr[i]);
	}
	for(int i=totalTask;i<MAX_TASKS; i++){
		gtk_widget_hide(label_array_widget_ptr[i]);
		gtk_widget_hide(compute_array_widget_ptr[i]);
		gtk_widget_hide(period_array_widget_ptr[i]);
	}
}

void on_button1_clicked (GtkButton *b){
	int Compute[6]; int Period[6];
	for(int i=0;i<totalTask; i++){
		Compute[i] = gtk_spin_button_get_value (GTK_SPIN_BUTTON(compute_array_widget_ptr[i]));
		Period[i] = gtk_spin_button_get_value (GTK_SPIN_BUTTON(period_array_widget_ptr[i]));
	}
	gboolean RM = gtk_switch_get_active(GTK_SWITCH(SwEnable_RM)); //
 	gboolean EDF = gtk_switch_get_active(GTK_SWITCH(SwEnable_EDF)); //
 	gboolean LLF = gtk_switch_get_active(GTK_SWITCH(SwEnable_LLF));

 	gboolean Display_format = gtk_switch_get_active(GTK_SWITCH(SwDisplay1)); //False=Default-> All together, True-> Single slide
	FILE *out_file = fopen("sys.txt", "w"); // write only
        // write to file vs write to screen
	for(int i=0;i<totalTask;i++){
		fprintf(out_file, "%d %d\n", Compute[i], Period[i]); // write to file
	}
	fclose(out_file);
	//it is needed to perform array of traces
	GArray *array_sim_traces = g_array_new(FALSE,FALSE,sizeof(SC_SimTrace));
	if(RM) {
		SC_SimTrace sim_trace = {NULL, false, 0, 1, 0, SC_RM_ID}; //RM
		sim_trace.trace = g_array_new(FALSE,FALSE,sizeof(int));
		g_array_append_val(array_sim_traces,sim_trace);
	}
	if(EDF){
		SC_SimTrace sim_trace = {NULL, false, 0, 1, 0, SC_EDF_ID}; //RM
		sim_trace.trace = g_array_new(FALSE,FALSE,sizeof(int));
		g_array_append_val(array_sim_traces,sim_trace);
	}
	if(LLF){
		SC_SimTrace sim_trace = {NULL, false, 0, 1, 0, SC_LLS_ID}; //RM
		sim_trace.trace = g_array_new(FALSE,FALSE,sizeof(int));
		g_array_append_val(array_sim_traces,sim_trace);
	}
	for (int i=0;i<array_sim_traces->len;i++){
		SC_Policy schedulable_policy = assign_policy(g_array_index(array_sim_traces,SC_SimTrace,i).policy_id);
		run_simulation(&g_array_index(array_sim_traces,SC_SimTrace,i),schedulable_policy);
		print_trace(&g_array_index(array_sim_traces,SC_SimTrace,i));
	}

	plot_array_trace_pdf(array_sim_traces,Display_format);

	for(int i=0;i<array_sim_traces->len;i++){
		g_array_free(g_array_index(array_sim_traces,SC_SimTrace,i).trace,TRUE);
	}
	gtk_widget_destroy(GTK_WIDGET(window));
}


// Ejecutar main task
int main(int argc, char** argv){
	 gtk_init(&argc, &argv); //init Gtk
	//---------------------------------
	// XML COnnection
	// ---------------------------------
    builder = gtk_builder_new_from_file ("mainForm.glade");
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, NULL);

    //get pointers to widgets in the window
    Gtk_Fixed = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));
	char string_names_buffer[1024];
	for(int i = 0; i<MAX_TASKS; i++){
		g_snprintf(string_names_buffer,1024,"label%d",i+1);
		label_array_widget_ptr[i] = GTK_WIDGET(gtk_builder_get_object(builder, string_names_buffer));
		gtk_widget_hide(label_array_widget_ptr[i]);

		g_snprintf(string_names_buffer,1024,"compute%d",i+1);
		compute_array_widget_ptr[i] = GTK_WIDGET(gtk_builder_get_object(builder, string_names_buffer));
		gtk_widget_hide(compute_array_widget_ptr[i]);

		g_snprintf(string_names_buffer,1024,"period%d",i+1);
		period_array_widget_ptr[i] = GTK_WIDGET(gtk_builder_get_object(builder, string_names_buffer));
		gtk_widget_hide(period_array_widget_ptr[i]);
	}
    SwDisplay1 = GTK_WIDGET(gtk_builder_get_object(builder, "SwDisplay1"));
	SwEnable_RM = GTK_WIDGET(gtk_builder_get_object(builder, "SwEnable1"));
	SwEnable_EDF = GTK_WIDGET(gtk_builder_get_object(builder, "SwEnable2"));
	SwEnable_LLF = GTK_WIDGET(gtk_builder_get_object(builder, "SwEnable3"));
    button1 = GTK_WIDGET(gtk_builder_get_object(builder, "button1"));
	TotalTask = GTK_WIDGET(gtk_builder_get_object(builder, "TotalTask"));
	Enter_button = GTK_WIDGET(gtk_builder_get_object(builder, "Enter"));

    gtk_widget_show(window);
    gtk_main();
    return 0;
}
