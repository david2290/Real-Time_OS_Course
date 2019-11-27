#include<stdbool.h>
#include<gmodule.h>

typedef struct SC_Process{
	int compute_time;
	int period_time;
} SC_Process;

typedef struct SC_SimTrace{
	GArray *trace;
	int error;
	int incomplete_task_id;
	float utilization;
} SC_SimTrace;

typedef int (*SC_Policy)(GArray*, GArray*, int);

//void SC_createSystemFromFile(GArray *);
int SC_calc_greatest_common_multiple(int,int);
int SC_calc_least_common_multiple(GArray *);
int SC_RM_policy(GArray *, GArray *, int);
int SC_EDF_policy(GArray *, GArray *, int);
int SC_LLS_policy(GArray *, GArray *, int);
void SC_refresh_deadlines_accordingly(GArray *, GArray *);
void SC_found_schedule_error(GArray *, SC_SimTrace*);
void SC_run_step(GArray *, int, int, SC_SimTrace* );
void SC_schedule(GArray *,SC_Policy, SC_SimTrace*);
float SC_calc_cpu_utilization(GArray *);
