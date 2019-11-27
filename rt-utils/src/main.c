#include <stdio.h>
#include <stdlib.h>
#include "latex_tools.h"
#include "schedule-utils.h"

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

int main(int argc, char** argv){
	SC_SimTrace sim_trace = {NULL, false, 0, 1};
	sim_trace.trace = g_array_new(FALSE,FALSE,sizeof(int));
	SC_Policy schedulable_policy = SC_RM_policy;
	run_simulation(&sim_trace,schedulable_policy);
	print_trace(&sim_trace);
	g_array_free(sim_trace.trace,TRUE);
    return 0;
}
