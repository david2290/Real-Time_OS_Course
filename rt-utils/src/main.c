#include <stdio.h>
#include <stdlib.h>
#include "latex_tools.h"
#include "schedule-utils.h"
//
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <math.h>
#include <ctype.h>

//***************** GTK ***************************

GtkWidget     *window;
GtkWidget     *Gtk_Fixed;
GtkWidget     *compute1;
GtkWidget     *compute2;
GtkWidget     *compute3;
GtkWidget     *compute4;
GtkWidget     *compute5;
GtkWidget     *compute6;
GtkWidget     *period1;
GtkWidget     *period2;
GtkWidget     *period3;
GtkWidget     *period4;
GtkWidget     *period5;
GtkWidget     *period6;
GtkWidget     *button1;
GtkWidget     *Switch1;
GtkWidget     *Switch2;
GtkWidget     *Switch3;
GtkWidget     *Switch4;
GtkWidget     *Switch5;
GtkWidget     *Switch6;
GtkWidget     *SwEnable1;
GtkWidget     *SwEnable2;
GtkWidget     *SwEnable3;
GtkBuilder    *SwDisplay1;
GtkWidget     *label1;
GtkWidget     *label2;
GtkWidget     *label3;
GtkWidget     *label4;
GtkWidget     *label5;
GtkWidget     *label6;
GtkWidget     *TotalTask;
GtkWidget     *Enter;
GtkBuilder    *builder;

void SC_createSystemFromFile(GArray *readyQueue){
    FILE *InputFile = fopen("sys.txt", "r");
	while(!feof(InputFile)){
		SC_Process readProcess={0,0};
fscanf(InputFile, "%d %d",
			&readProcess.compute_time,
			&readProcess.period_time);
		if(readProcess.period_time>0)
			g_array_append_val(readyQueue, readProcess);
    }
}

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

void plot_trace_pdf(){
	RT_create_from_base_file();
	RT_export_project();
	RT_open_window();
}

void run_simulation(SC_SimTrace *struct_trace_ptr, SC_Policy schedulable_policy){
	GArray *readyQueue = g_array_new (FALSE, FALSE, sizeof(SC_Process));
    SC_createSystemFromFile(readyQueue);
	float utilization = SC_calc_cpu_utilization(readyQueue);
	struct_trace_ptr->utilization=utilization;
	if(utilization>1){
		struct_trace_ptr->error=true;
	}else{
		SC_schedule(readyQueue,schedulable_policy,struct_trace_ptr);
	}
	g_array_free(readyQueue, TRUE);
}


// Ejecutar main task

void on_Enter_clicked (GtkButton *c){
	gdouble totalTask = gtk_spin_button_get_value (GTK_SPIN_BUTTON(TotalTask)); 
	if (totalTask==1.0) {gtk_widget_show(label1); gtk_widget_show(compute1); gtk_widget_show(period1);
	gtk_widget_hide(label2); gtk_widget_hide(compute2); gtk_widget_hide(period2);
	gtk_widget_hide(label3); gtk_widget_hide(compute3); gtk_widget_hide(period3);
	gtk_widget_hide(label4); gtk_widget_hide(compute4); gtk_widget_hide(period4);
	gtk_widget_hide(label5); gtk_widget_hide(compute5); gtk_widget_hide(period5);
	gtk_widget_hide(label6); gtk_widget_hide(compute6); gtk_widget_hide(period6);}
	if (totalTask==2.0) {gtk_widget_show(label1); gtk_widget_show(compute1); gtk_widget_show(period1);
	gtk_widget_show(label2); gtk_widget_show(compute2); gtk_widget_show(period2);
	gtk_widget_hide(label3); gtk_widget_hide(compute3); gtk_widget_hide(period3);
	gtk_widget_hide(label4); gtk_widget_hide(compute4); gtk_widget_hide(period4);
	gtk_widget_hide(label5); gtk_widget_hide(compute5); gtk_widget_hide(period5);
	gtk_widget_hide(label6); gtk_widget_hide(compute6); gtk_widget_hide(period6);}
	if (totalTask==3.0) {gtk_widget_show(label1); gtk_widget_show(compute1); gtk_widget_show(period1);
	gtk_widget_show(label2); gtk_widget_show(compute2); gtk_widget_show(period2);
	gtk_widget_show(label3); gtk_widget_show(compute3); gtk_widget_show(period3);
	gtk_widget_hide(label4); gtk_widget_hide(compute4); gtk_widget_hide(period4);
	gtk_widget_hide(label5); gtk_widget_hide(compute5); gtk_widget_hide(period5);
	gtk_widget_hide(label6); gtk_widget_hide(compute6); gtk_widget_hide(period6);}
	if (totalTask==4.0) {gtk_widget_show(label1); gtk_widget_show(compute1); gtk_widget_show(period1);
	gtk_widget_show(label2); gtk_widget_show(compute2); gtk_widget_show(period2);
	gtk_widget_show(label3); gtk_widget_show(compute3); gtk_widget_show(period3);
	gtk_widget_show(label4); gtk_widget_show(compute4); gtk_widget_show(period4);
	gtk_widget_hide(label5); gtk_widget_hide(compute5); gtk_widget_hide(period5);
	gtk_widget_hide(label6); gtk_widget_hide(compute6); gtk_widget_hide(period6);}
	if (totalTask==5.0) {gtk_widget_show(label1); gtk_widget_show(compute1); gtk_widget_show(period1);
	gtk_widget_show(label2); gtk_widget_show(compute2); gtk_widget_show(period2);
	gtk_widget_show(label3); gtk_widget_show(compute3); gtk_widget_show(period3);
	gtk_widget_show(label4); gtk_widget_show(compute4); gtk_widget_show(period4);
	gtk_widget_show(label5); gtk_widget_show(compute5); gtk_widget_show(period5);
	gtk_widget_hide(label6); gtk_widget_hide(compute6); gtk_widget_hide(period6);}
	if (totalTask==6.0) {gtk_widget_show(label1); gtk_widget_show(compute1); gtk_widget_show(period1);
	gtk_widget_show(label2); gtk_widget_show(compute2); gtk_widget_show(period2);
	gtk_widget_show(label3); gtk_widget_show(compute3); gtk_widget_show(period3);
	gtk_widget_show(label4); gtk_widget_show(compute4); gtk_widget_show(period4);
	gtk_widget_show(label5); gtk_widget_show(compute5); gtk_widget_show(period5);
	gtk_widget_show(label6); gtk_widget_show(compute6); gtk_widget_show(period6);}

}

void on_button1_clicked (GtkButton *b){
	gdouble Compute[6]; gdouble Period[6];
	Compute[0] = gtk_spin_button_get_value (GTK_SPIN_BUTTON(compute1)); //
	Compute[1] = gtk_spin_button_get_value (GTK_SPIN_BUTTON(compute2)); //
	Compute[2] = gtk_spin_button_get_value (GTK_SPIN_BUTTON(compute3)); //
	Compute[3] = gtk_spin_button_get_value (GTK_SPIN_BUTTON(compute4)); //
	Compute[4] = gtk_spin_button_get_value (GTK_SPIN_BUTTON(compute5)); //
	Compute[5] = gtk_spin_button_get_value (GTK_SPIN_BUTTON(compute6)); //
	Period[0] = gtk_spin_button_get_value (GTK_SPIN_BUTTON(period1)); // 
	Period[1] = gtk_spin_button_get_value (GTK_SPIN_BUTTON(period2)); //
	Period[2] = gtk_spin_button_get_value (GTK_SPIN_BUTTON(period3)); //
	Period[3] = gtk_spin_button_get_value (GTK_SPIN_BUTTON(period4)); //
	Period[4] = gtk_spin_button_get_value (GTK_SPIN_BUTTON(period5)); //
	Period[5] = gtk_spin_button_get_value (GTK_SPIN_BUTTON(period6)); //

	gboolean RM = gtk_switch_get_active(GTK_SWITCH(SwEnable1)); // 
 	gboolean EDF = gtk_switch_get_active(GTK_SWITCH(SwEnable2)); // 
 	gboolean LLF = gtk_switch_get_active(GTK_SWITCH(SwEnable3)); //
 	gboolean Display = gtk_switch_get_active(GTK_SWITCH(SwDisplay1)); //
	FILE *out_file = fopen("sys.txt", "w"); // write only
        // write to file vs write to screen
	for(int i=0;i<2;i++){	//FALTA hacerlo dinamico
		fprintf(out_file, "%d %d\n", (int)Compute[i], (int)Period[i]); // write to file
	}
	fclose(out_file);

	SC_SimTrace sim_trace = {NULL, false, 0, 1};
	sim_trace.trace = g_array_new(FALSE,FALSE,sizeof(int));
	SC_Policy schedulable_policy = SC_RM_policy;
	run_simulation(&sim_trace,schedulable_policy);
	print_trace(&sim_trace);
	g_array_free(sim_trace.trace,TRUE);
}

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
        label1 = GTK_WIDGET(gtk_builder_get_object(builder, "label1"));
        label2 = GTK_WIDGET(gtk_builder_get_object(builder, "label2"));
        label3 = GTK_WIDGET(gtk_builder_get_object(builder, "label3"));
        label4 = GTK_WIDGET(gtk_builder_get_object(builder, "label4"));
        label5 = GTK_WIDGET(gtk_builder_get_object(builder, "label5"));
        label6 = GTK_WIDGET(gtk_builder_get_object(builder, "label6"));
        compute1 = GTK_WIDGET(gtk_builder_get_object(builder, "compute1"));
        compute2 = GTK_WIDGET(gtk_builder_get_object(builder, "compute2"));
        compute3 = GTK_WIDGET(gtk_builder_get_object(builder, "compute3"));
        compute4 = GTK_WIDGET(gtk_builder_get_object(builder, "compute4"));
        compute5 = GTK_WIDGET(gtk_builder_get_object(builder, "compute5"));
        compute6 = GTK_WIDGET(gtk_builder_get_object(builder, "compute6"));
        SwDisplay1 = GTK_WIDGET(gtk_builder_get_object(builder, "SwDisplay1"));
        period1 = GTK_WIDGET(gtk_builder_get_object(builder, "period1"));
        period2 = GTK_WIDGET(gtk_builder_get_object(builder, "period2"));
        period3 = GTK_WIDGET(gtk_builder_get_object(builder, "period3"));
        period4 = GTK_WIDGET(gtk_builder_get_object(builder, "period4"));
        period5 = GTK_WIDGET(gtk_builder_get_object(builder, "period5"));
        period6 = GTK_WIDGET(gtk_builder_get_object(builder, "period6"));
        Switch1 = GTK_WIDGET(gtk_builder_get_object(builder, "Switch1"));
        Switch2 = GTK_WIDGET(gtk_builder_get_object(builder, "Switch2"));
        Switch3 = GTK_WIDGET(gtk_builder_get_object(builder, "Switch3"));
        Switch4 = GTK_WIDGET(gtk_builder_get_object(builder, "Switch4"));
        Switch5 = GTK_WIDGET(gtk_builder_get_object(builder, "Switch5"));
        Switch6 = GTK_WIDGET(gtk_builder_get_object(builder, "Switch6"));
        SwEnable1 = GTK_WIDGET(gtk_builder_get_object(builder, "SwEnable1"));
        SwEnable2 = GTK_WIDGET(gtk_builder_get_object(builder, "SwEnable2"));
        SwEnable3 = GTK_WIDGET(gtk_builder_get_object(builder, "SwEnable3"));
        button1 = GTK_WIDGET(gtk_builder_get_object(builder, "button1"));
	TotalTask = GTK_WIDGET(gtk_builder_get_object(builder, "TotalTask"));
	Enter = GTK_WIDGET(gtk_builder_get_object(builder, "Enter"));
	gtk_widget_hide(label1); gtk_widget_hide(compute1); gtk_widget_hide(period1);
	gtk_widget_hide(label2); gtk_widget_hide(compute2); gtk_widget_hide(period2);
	gtk_widget_hide(label3); gtk_widget_hide(compute3); gtk_widget_hide(period3);
	gtk_widget_hide(label4); gtk_widget_hide(compute4); gtk_widget_hide(period4);
	gtk_widget_hide(label5); gtk_widget_hide(compute5); gtk_widget_hide(period5);
	gtk_widget_hide(label6); gtk_widget_hide(compute6); gtk_widget_hide(period6);
        gtk_widget_show(window);
        gtk_main();
    return 0;
}
