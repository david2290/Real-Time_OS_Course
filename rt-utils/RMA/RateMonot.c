#include<stdio.h>
#include<stdbool.h>
#include<gmodule.h>

typedef struct Process{
	int compute_time;
	int period_time;
} Process;

typedef int (*Policy)(GArray*, GArray*, int);

void createSystemFromFile(GArray *);
int calc_greatest_common_multiple(int,int);
int calc_least_common_multiple(GArray *);
int RM_policy(GArray *, GArray *, int);
int EDF_policy(GArray *, GArray *, int);
int LLS_policy(GArray *, GArray *, int);
void refresh_deadlines_accordingly(GArray *, GArray *);
bool found_schedule_error(GArray *);
void print_schedule_step_output(GArray *, int, int);
void schedule(GArray *,Policy);
float calc_cpu_utilization(GArray *);


int main(){
	GArray *readyQueue = g_array_new (FALSE, FALSE, sizeof(Process));
    createSystemFromFile(readyQueue);
	float utilization = calc_cpu_utilization(readyQueue);
	Policy schedulable_policy = LLS_policy;
	if(utilization>1)
		printf("\nthis system isn't schedulable because\nthe sum of Ei/Pi = %f > 1\n",utilization);
	else
	    schedule(readyQueue,schedulable_policy);
	printf("\n");
	g_array_free(readyQueue, TRUE);
}

void createSystemFromFile(GArray *readyQueue){
    FILE *InputFile = fopen("sys.txt", "r");
	while(!feof(InputFile)){
		Process readProcess={0,0};
        fscanf(InputFile, "%d %d",
			&readProcess.compute_time,
			&readProcess.period_time);
		if(readProcess.period_time>0)
			g_array_append_val(readyQueue, readProcess);
    }
}

int calc_greatest_common_multiple(int a,int b){
	if (b == 0) return a;
	return calc_greatest_common_multiple(b, a % b);
}

int calc_least_common_multiple(GArray *readyQueue){
	int length = (*readyQueue).len;
	int result = g_array_index(readyQueue,Process,0).period_time;
    for (int i = 0; i < length; i++){
		int i_number = g_array_index(readyQueue,Process,i).period_time;
		int gcd = calc_greatest_common_multiple(i_number, result);
		result = (i_number*result)/gcd;
	}
    return result;
}

int RM_policy(GArray *simulationState, GArray *readyQueue, int lcm){
    // this function returns the id of the prior process and returns -1 if no process is ready to execute
	const int numberOfProcesses = (*simulationState).len;
    int choosen_task = -1, smallerPeriod = lcm;
    for(int i=0; i<numberOfProcesses; i++){
		int compute_time = g_array_index(simulationState,Process,i).compute_time;
		int period_time = g_array_index(readyQueue,Process,i).period_time;
		if(compute_time > 0 && smallerPeriod >= period_time){
			choosen_task = i;
			smallerPeriod = period_time;
		}
    }
    return choosen_task;
}

int EDF_policy(GArray *simulationState, GArray *readyQueue, int lcm){
    // this function returns the id of the prior process and returns -1 if no process is ready to execute
	const int numberOfProcesses = (*simulationState).len;
    int choosen_task = -1, smallerDeadline = lcm;
    for(int i=0; i<numberOfProcesses; i++){
		int compute_time = g_array_index(simulationState,Process,i).compute_time;
		int period_time = g_array_index(simulationState,Process,i).period_time;
		if(compute_time > 0 && smallerDeadline >= period_time){
			choosen_task = i;
			smallerDeadline = period_time;
		}
    }
    return choosen_task;
}

int LLS_policy(GArray *simulationState, GArray *readyQueue, int lcm){
    // this function returns the id of the prior process and returns -1 if no process is ready to execute
	const int numberOfProcesses = (*simulationState).len;
    int choosen_task = -1, smallerLaxity = lcm;
    for(int i=0; i<numberOfProcesses; i++){
		int compute_time = g_array_index(simulationState,Process,i).compute_time;
		int period_time = g_array_index(simulationState,Process,i).period_time;
		int laxity = period_time - compute_time;
		if(compute_time > 0 && smallerLaxity >= laxity){
			choosen_task = i;
			smallerLaxity = laxity;
		}
    }
    return choosen_task;
}


void refresh_deadlines_accordingly(GArray *simulationState, GArray *readyQueue){
	for(int i=0; i<(*simulationState).len; i++){
		g_array_index(simulationState,Process,i).period_time--;
		if(g_array_index(simulationState,Process,i).period_time == 0 &&
			g_array_index(simulationState,Process,i).compute_time == 0){
			g_array_index(simulationState,Process,i) = g_array_index(readyQueue,Process,i);
		}
	}
}

bool found_schedule_error(GArray *simulationState){
	for(int i= 0; i< (*simulationState).len; i++){
		int compute_time = g_array_index(simulationState,Process,i).compute_time;
		int period_time = g_array_index(simulationState,Process,i).period_time;
		printf(" c:%d d:%d ", compute_time, period_time);
		if(compute_time > period_time){
			printf("\nthe system is not schedulable because the task %d cant finish its job in its specified period",i);
			return true;
		}
	}
	return false;
}

void print_schedule_step_output(GArray *simulationState, int running_process_index, int t){
	if(running_process_index != -1){
		printf("\n(%d-%d)| t%d",t,t+1, running_process_index);
		g_array_index(simulationState,Process,running_process_index).compute_time--;
	}
	else{
		printf("\n(%d-%d)",t,t+1);
	}
}

void schedule(GArray *readyQueue, Policy schedule_policy){
	int numberOfProcesses = (*readyQueue).len;
	int lcm = calc_least_common_multiple(readyQueue);
	GArray *simulationState = g_array_new(FALSE,FALSE,sizeof(Process));
	g_array_append_vals(simulationState,(*readyQueue).data,(*readyQueue).len);
	int running_process_index;
    for(int t=0; t<lcm; t++){
		if(found_schedule_error(simulationState)) return;
		running_process_index = schedule_policy(simulationState,readyQueue,lcm);
		print_schedule_step_output(simulationState,running_process_index,t);
		refresh_deadlines_accordingly(simulationState,readyQueue);
	}
	g_array_free(simulationState,TRUE);
}

float calc_cpu_utilization(GArray *readyQueue){
	double utilization_acc=0.0f;
	int numberOfProcesses = (*readyQueue).len;
	for(int i=0; i < numberOfProcesses; i++){
		double compute_time = g_array_index(readyQueue,Process,i).compute_time;
		double period_time = g_array_index(readyQueue,Process,i).period_time;
		utilization_acc += compute_time/period_time;
	}
	float utilization = utilization_acc;
	return utilization;
}

// gcc RateMonot.c -o RMA `pkg-config --cflags --libs glib-2.0` `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`
