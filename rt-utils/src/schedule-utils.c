#include<stdio.h>
#include<stdbool.h>
#include<gmodule.h>
#include "schedule-utils.h"

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

int SC_calc_greatest_common_multiple(int a,int b){
	if (b == 0) return a;
	return SC_calc_greatest_common_multiple(b, a % b);
}

int SC_calc_least_common_multiple(GArray *readyQueue){
	int length = readyQueue->len;
	int result = g_array_index(readyQueue,SC_Process,0).period_time;
    for (int i = 0; i < length; i++){
		int i_number = g_array_index(readyQueue,SC_Process,i).period_time;
		int gcd = SC_calc_greatest_common_multiple(i_number, result);
		result = (i_number*result)/gcd;
	}
    return result;
}

int SC_RM_policy(GArray *simulationState, GArray *readyQueue, int lcm){
    // this function returns the id of the prior process and returns -1 if no process is ready to execute
	const int numberOfProcesses = simulationState->len;
    int choosen_task = -1, smallerPeriod = lcm;
    for(int i=0; i<numberOfProcesses; i++){
		int compute_time = g_array_index(simulationState,SC_Process,i).compute_time;
		int period_time = g_array_index(readyQueue,SC_Process,i).period_time;
		if(compute_time > 0 && smallerPeriod >= period_time){
			choosen_task = i;
			smallerPeriod = period_time;
		}
    }
    return choosen_task;
}

int SC_EDF_policy(GArray *simulationState, GArray *readyQueue, int lcm){
    // this function returns the id of the prior process and returns -1 if no process is ready to execute
	const int numberOfProcesses = simulationState->len;
    int choosen_task = -1, smallerDeadline = lcm;
    for(int i=0; i<numberOfProcesses; i++){
		int compute_time = g_array_index(simulationState,SC_Process,i).compute_time;
		int period_time = g_array_index(simulationState,SC_Process,i).period_time;
		if(compute_time > 0 && smallerDeadline >= period_time){
			choosen_task = i;
			smallerDeadline = period_time;
		}
    }
    return choosen_task;
}

int SC_LLS_policy(GArray *simulationState, GArray *readyQueue, int lcm){
    // this function returns the id of the prior process and returns -1 if no process is ready to execute
	const int numberOfProcesses = simulationState->len;
    int choosen_task = -1, smallerLaxity = lcm;
    for(int i=0; i<numberOfProcesses; i++){
		int compute_time = g_array_index(simulationState,SC_Process,i).compute_time;
		int period_time = g_array_index(simulationState,SC_Process,i).period_time;
		int laxity = period_time - compute_time;
		if(compute_time > 0 && smallerLaxity >= laxity){
			choosen_task = i;
			smallerLaxity = laxity;
		}
    }
    return choosen_task;
}


void SC_refresh_deadlines_accordingly(GArray *simulationState, GArray *readyQueue){
	for(int i=0; i<simulationState->len; i++){
		g_array_index(simulationState,SC_Process,i).period_time--;
		if(g_array_index(simulationState,SC_Process,i).period_time == 0 &&
			g_array_index(simulationState,SC_Process,i).compute_time == 0){
			g_array_index(simulationState,SC_Process,i) = g_array_index(readyQueue,SC_Process,i);
		}
	}
}

void SC_found_schedule_error(GArray *simulationState,SC_SimTrace *struct_trace_ptr){
	for(int i= 0; i< simulationState->len; i++){
		int compute_time = g_array_index(simulationState,SC_Process,i).compute_time;
		int period_time = g_array_index(simulationState,SC_Process,i).period_time;
		if(compute_time > period_time){
			struct_trace_ptr->error = true;
			struct_trace_ptr->incomplete_task_id = i;
			return;
		}
	}
}

void SC_run_step(GArray *simulationState, int running_process_index, int t, SC_SimTrace* struct_trace_ptr){
	g_array_append_val(struct_trace_ptr->trace,running_process_index);
	if(running_process_index != -1){
		g_array_index(simulationState,SC_Process,running_process_index).compute_time--;
	}
}

void SC_schedule(GArray *readyQueue, SC_Policy schedule_policy, SC_SimTrace* struct_trace_ptr){
	int lcm = SC_calc_least_common_multiple(readyQueue);
	printf("lcm: %d  \n",lcm);
	GArray *simulationState = g_array_new(FALSE,FALSE,sizeof(SC_Process));
	g_array_append_vals(simulationState,readyQueue->data,readyQueue->len);
	int running_process_index;
    for(int t=0; t<lcm; t++){
		SC_found_schedule_error(simulationState,struct_trace_ptr);
		if(struct_trace_ptr-> error)return;
		running_process_index = schedule_policy(simulationState,readyQueue,lcm);
		SC_run_step(simulationState,running_process_index,t,struct_trace_ptr);
		SC_refresh_deadlines_accordingly(simulationState,readyQueue);
	}
	g_array_free(simulationState,TRUE);
}

float SC_calc_cpu_utilization(GArray *readyQueue){
	double utilization_acc=0.0f;
	int numberOfProcesses = readyQueue->len;
	for(int i=0; i < numberOfProcesses; i++){
		double compute_time = g_array_index(readyQueue,SC_Process,i).compute_time;
		double period_time = g_array_index(readyQueue,SC_Process,i).period_time;
		utilization_acc += compute_time/period_time;
	}
	float utilization = utilization_acc;
	return utilization;
}
