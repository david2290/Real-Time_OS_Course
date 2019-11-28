#include <stdio.h>
#include <stdlib.h>
#include "schedule-utils.h"
#include "latex_tools.h"

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

void plot_array_trace_pdf(GArray *struct_trace_ptr){
	FILE* file = RT_create_file_buffer();
	RT_print_trace(struct_trace_ptr,file);
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

int main(int argc, char** argv){
	GArray *array_sim_traces = g_array_new(FALSE,FALSE,sizeof(SC_SimTrace));
	SC_SimTrace sim_trace = {NULL, false, 0, 1, 0, SC_RM_ID};
	sim_trace.trace = g_array_new(FALSE,FALSE,sizeof(int));
	g_array_append_val(array_sim_traces,sim_trace);
	sim_trace.policy_id = SC_EDF_ID;
	sim_trace.trace = g_array_new(FALSE,FALSE,sizeof(int));
	g_array_append_val(array_sim_traces,sim_trace);
	sim_trace.policy_id = SC_LLS_ID;
	sim_trace.trace = g_array_new(FALSE,FALSE,sizeof(int));
	g_array_append_val(array_sim_traces,sim_trace);
	for (int i=0;i<array_sim_traces->len;i++){
		SC_Policy schedulable_policy = assign_policy(g_array_index(array_sim_traces,SC_SimTrace,i).policy_id);
		run_simulation(&g_array_index(array_sim_traces,SC_SimTrace,i),schedulable_policy);
		print_trace(&g_array_index(array_sim_traces,SC_SimTrace,i));
	}
	plot_array_trace_pdf(array_sim_traces);
	for(int i=0;i<array_sim_traces->len;i++){
		g_array_free(g_array_index(array_sim_traces,SC_SimTrace,i).trace,TRUE);
	}
    return 0;
}
