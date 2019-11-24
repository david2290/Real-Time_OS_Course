#include<stdio.h>
#include<stdbool.h>
#include<gmodule.h>

typedef struct Process{
	int compute_time;
	int period_time;
} Process;

int realTimeCapacity[2][100];
void createSystemFromFile(GArray *readyQueue){
    FILE *InputFile = fopen("sys.txt", "r");
	int processesNumber = 0;
	while(!feof(InputFile)){
		Process readProcess={0,0};
        fscanf(InputFile, "%d %d",
			&readProcess.compute_time,
			&readProcess.period_time);
		g_array_append_val(readyQueue, readProcess);
        processesNumber++;
    }
}

int calc_greatest_common_multiple(int a,int b){
	if (b == 0) return a;
	return calc_greatest_common_multiple(b, a % b);
}

int calc_least_common_multiple(GArray *readyQueue){
	int length = (*readyQueue).len;
	int result = g_array_index(readyQueue,Process,0).period_time;
    for (int i = 1; i < length; i++){
		int i_number = g_array_index(readyQueue,Process,i).period_time;
		int gcd = calc_greatest_common_multiple(i_number, result);
		result = (i_number*result)/gcd;
	}
    return result;
}


int RM_policy(GArray *simulationState, int lcm){
    // this function returns the id of the prior process and returns -1 if no process is ready to execute
	const int numberOfProcesses = (*simulationState).len;
    int prior = -1, lessPeriode = lcm;
    for(int i=0; i<numberOfProcesses; i++){
		int compute_time = g_array_index(simulationState,Process,i).compute_time;
		int period_time = g_array_index(simulationState,Process,i).period_time;
		if(compute_time > 0){
			if(lessPeriode > period_time){
				prior = i;
				lessPeriode = period_time;
			}
		}
    }
    return prior;
}

void refresh_deadlines_accordingly(GArray *simulationState, GArray *readyQueue){
	for(int i=0; i<(*simulationState).len; i++){
		g_array_index(simulationState,Process,i).period_time--;
		if(g_array_index(simulationState,Process,i).period_time == 0){
			g_array_index(simulationState,Process,i) = g_array_index(readyQueue,Process,i);
		}
	}
}

bool found_schedule_error(GArray *simulationState){
	for(int i= 0; i< (*simulationState).len; i++){
		if(g_array_index(simulationState,Process,i).compute_time
			> g_array_index(simulationState,Process,i).period_time){
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

void schedule(GArray *readyQueue){
	int numberOfProcesses = (*readyQueue).len;
	int lcm = calc_least_common_multiple(readyQueue);
	GArray *simulationState = g_array_new(FALSE,FALSE,sizeof(Process));
	g_array_append_vals(simulationState,(*readyQueue).data,(*readyQueue).len);
	int running_process_index;
    for(int t=0; t<lcm; t++){
		if(found_schedule_error(simulationState)) return;
		running_process_index = RM_policy(simulationState,lcm);
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

int main(){
	GArray *readyQueue = g_array_new (FALSE, FALSE, sizeof(Process));
    int x,help[100];
    createSystemFromFile(readyQueue);
    //schedulability...
	float utilization = calc_cpu_utilization(readyQueue);
	if(utilization>1){
		printf("\nthis system isn't schedulable because\nthe sum of Ei/Pi = %f > 1\n",utilization);
	}
	else{
	    schedule(readyQueue);
	}
	printf("End of the simulation\n");
	g_array_free(readyQueue, TRUE);
}

// gcc RateMonot.c -o RMA `pkg-config --cflags --libs glib-2.0` `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`
