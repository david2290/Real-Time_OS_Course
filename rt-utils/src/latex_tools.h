#define FILE_NAME "output"

void RT_create_template_file();
void RT_print_trace(GArray *, FILE* f);
void RT_export_pdf();
void RT_open_window();
void RT_latex_frame_title_string(FILE *, int);
void RT_latex_frame_algorithm_info(FILE*,int,float);
void RT_latex_packages(FILE*);
void RT_latex_author_info(FILE *);
void RT_latex_first_frames(GArray *, FILE*);
void RT_latex_title_string(FILE *, int, int);
void RT_latex_ganttbar_from_trace(SC_SimTrace *, FILE* , int);
void RT_latex_simulation_frame_per_algorithm(SC_SimTrace *, FILE*);
void RT_print_trace(GArray *, FILE*);


FILE* RT_create_file_buffer(){
	return fopen(FILE_NAME".tex","w");
}

void RT_latex_frame_title_string(FILE *f, int policy_id){
	switch (policy_id) {
		case SC_RM_ID:
			fprintf(f, "\\frametitle{Rate Monotonic Scheduling}\n");
		break;
		case SC_EDF_ID:
			fprintf(f, "\\frametitle{Earliest Deadline Scheduling}\n");
		break;
		case SC_LLS_ID:
			fprintf(f, "\\frametitle{Least Laxity Scheduling}\n");
		break;
		default: break;
	}
}

void RT_latex_frame_algorithm_info(FILE*f,int policy_id,float utilization){
	switch (policy_id) {
		case SC_RM_ID:
			fprintf(f, "\\begin{equation}\n");
			fprintf(f, "\\sum_{i=0}^N \\frac{C_i}{D_i} = %f \\leq U(N) = N\\left( 2^{-N} -1 \\right)\n",utilization);
			fprintf(f, "\\end{equation}\n");
		break;
		case SC_EDF_ID:
			fprintf(f, "\\begin{equation}\n");
			fprintf(f, "\\sum_{i=0}^N \\frac{C_i}{D_i} = %f \\leq 1 \n",utilization);
			fprintf(f, "\\end{equation}\n");
		break;
		case SC_LLS_ID:
			fprintf(f, "\\begin{equation}\n");
			fprintf(f, "\\sum_{i=0}^N \\frac{C_i}{D_i} = %f \\leq 1\n",utilization);
			fprintf(f, "\\end{equation}\n");
		break;
		default: break;
	}
}

void RT_latex_packages(FILE* f){
	fprintf(f, "\\documentclass{beamer}\n");
	fprintf(f, "\\usepackage{tikz}\n");
	fprintf(f, "\\usepackage{pgfgantt}\n");
}

void RT_latex_author_info(FILE *f){
	fprintf(f, "\\title{Scheduler Simulator}\n");
	fprintf(f, "\\author{Kaleb Alfaro Badilla, David Ramírez Arroyo \\& Alejandro }\n");
	fprintf(f, "\\date{\\today}\n");
}

void RT_latex_first_frames(GArray *array_trace_ptr, FILE* f){
	fprintf(f, "\\frame{\\titlepage}\n");
	for(int i=0; i<array_trace_ptr->len;i++){
		fprintf(f, "\\begin{frame}\n");
		int policy_id = g_array_index(array_trace_ptr,SC_SimTrace,i).policy_id;
		float utilization = g_array_index(array_trace_ptr,SC_SimTrace,i).utilization;
		RT_latex_frame_title_string(f,policy_id);
		RT_latex_frame_algorithm_info(f,policy_id,utilization);
		fprintf(f, "\\end{frame}\n");
	}
}

void RT_latex_title_string(FILE *f, int policy_id, int part_number){
	switch (policy_id) {
		case SC_RM_ID:
			fprintf(f, "\t\\gantttitle{Rate Monotonic Scheduler %d}{40} \\\\ \n",part_number);
		break;
		case SC_EDF_ID:
			fprintf(f, "\t\\gantttitle{Earliest Deadline First Scheduler %d}{40} \\\\ \n",part_number);
		break;
		case SC_LLS_ID:
			fprintf(f, "\t\\gantttitle{Least Laxity First Scheduler %d}{40} \\\\ \n",part_number);
		break;
		default: break;
	}
}

void RT_latex_ganttbar_from_trace(SC_SimTrace *struct_trace_ptr, FILE* f, int start_from){
	int number_of_tasks = struct_trace_ptr->number_of_tasks;
	char **vector_string = g_new(char*,number_of_tasks);
	GArray *trace = struct_trace_ptr->trace;
	int remaining_steps =  trace->len - start_from;
	int t_steps = (40 < remaining_steps) ? (start_from + 40) : (start_from + remaining_steps);

	for(int i=0;i<number_of_tasks;i++)
		vector_string[i] = g_strdup_printf("");

	int task_runnning = g_array_index(trace,int, 0);
	int t_begin = 1;  int t_end = 1;

	for(int i=start_from; i<t_steps; i++){
		int next_task_runnning = g_array_index(trace,int, i);
		if(task_runnning!=next_task_runnning){
			if(task_runnning!=-1){
				t_begin=(t_begin%40==0)?40:t_begin%40;
				t_end=(i%40==0)?40:i%40;
				char* updated_msg=g_strdup_printf("%s\\ganttbar[bar/.append style={fill=pink}]{Task %d}{%d}{%d} ",
					vector_string[task_runnning],task_runnning,t_begin,t_end);
				g_free(vector_string[task_runnning]);
				vector_string[task_runnning] = updated_msg;
			}
			task_runnning=next_task_runnning;
			t_begin=i+1;
		}
	}

	if(task_runnning!=-1){
		t_end= (t_steps%40==0)?40:t_steps%40;
		t_begin=(t_begin%40==0)?40:t_begin%40;
		char* updated_msg = g_strdup_printf("%s\\ganttbar[bar/.append style={fill=pink}]{Task %d}{%d}{%d} ",
			vector_string[task_runnning],task_runnning,t_begin,t_end);
		g_free(vector_string[task_runnning]);
		vector_string[task_runnning] = updated_msg;
	}

	for(int i=0; i<struct_trace_ptr->number_of_tasks; i++){
		fprintf(f, "\t%s \\\\\n",vector_string[i]);
		g_free(vector_string[i]);
	}
	if(t_steps==struct_trace_ptr->trace->len){
		int step = (t_steps%40==0)?40:t_steps%40;
		if(struct_trace_ptr->error){
			fprintf(f, "\t\\ganttvrule[vrule/.append style={very thick,red}]{Missed the timing in task %d}{%d}\n", struct_trace_ptr->incomplete_task_id,step);
		}else{
			fprintf(f, "\t\\ganttvrule[vrule/.append style={very thick, red}]{End of simulation}{%d}\n",step);
		}
	}
	g_free(vector_string);
}

void RT_latex_simulation_frame_per_algorithm(SC_SimTrace *struct_trace_ptr, FILE* f){
	int number_of_frames = (struct_trace_ptr->trace->len / (40*3))+1;
	for(int frame_idx=0; frame_idx<number_of_frames; frame_idx++){
		fprintf(f, "\\begin{frame}\n");
		RT_latex_frame_title_string(f, struct_trace_ptr->policy_id);
		int remaining_gantts = ((struct_trace_ptr->trace->len%40)!=0)?((struct_trace_ptr->trace->len - (frame_idx*40*3))/40)+1:((struct_trace_ptr->trace->len - (frame_idx*40*3))/40);
		int number_of_gantts = (3<remaining_gantts) ? 3 : remaining_gantts;
		for(int j=0; j<number_of_gantts; j++){
			printf("\tj:%d \n", j);
			fprintf(f,"\t\\begin{ganttchart}[vgrid,x unit=0.25cm,y unit title=0.3cm,y unit chart=0.2cm,title label font=\\fontsize{3}{4}\\selectfont,bar label font=\\fontsize{3}{4}\\selectfont]{1}{40}\n");
			RT_latex_title_string(f,struct_trace_ptr->policy_id,j+(frame_idx*3));
			fprintf(f, "\t\\gantttitlelist{%d,...,%d}{1} \\\\\n",(frame_idx*40*3) + j*40 + 1, (frame_idx*40*3) + (j+1)*40 );
			RT_latex_ganttbar_from_trace(struct_trace_ptr,f, (frame_idx*40*3) + j*40);
			fprintf(f, "\t\\end{ganttchart}\n");
		}
		fprintf(f, "\\end{frame}\n");
	}
}

void RT_print_trace(GArray *array_trace_ptr, FILE* f){
	RT_latex_packages(f);
	fprintf(f, "\\begin{document}\n");
	RT_latex_author_info(f);
	RT_latex_first_frames(array_trace_ptr,f);
	for(int i=0; i<array_trace_ptr->len; i++){
		printf("trace len: %d\n", g_array_index(array_trace_ptr,SC_SimTrace,i).trace->len);
		RT_latex_simulation_frame_per_algorithm(&g_array_index(array_trace_ptr,SC_SimTrace,i), f);
	}
	fprintf(f, "\\end{document}\n");
}

void RT_export_pdf(){
	char pdfltx_cmd[] = "pdflatex "FILE_NAME".tex pgfgantt.sty";
	system(pdfltx_cmd);
}

void RT_open_window(){
	char pdf_viewer[] = "xdg-open "FILE_NAME".pdf &";
	system(pdf_viewer);
}
