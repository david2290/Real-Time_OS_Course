#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
------- CSV FIle Organization ------
Algoritm_Selection
XX_ms_Quatum,
XX_Processes,
Work_Units, TIme_X, X_Tickets
50, 100, 150
20, 2, 4
50, 100, 150
*/

int main(void) {
    FILE *fp = fopen("sample.csv", "r");
    if (!fp) {
        printf("Can't open file\n");
        return 0;
    }
char *line = NULL;
char *token;
size_t len = 0;
getline(&line, &len, fp);
token=strtok(line,"\n");
printf("%s\n",token);
getline(&line, &len, fp);
token=strtok(line,"\n");
printf("%s\n",token);
getline(&line, &len, fp);
token=strtok(line,"\n");
printf("%s\n",token);
int processes_num = atoi(token);
int var=0;
    for(int i=0;i<processes_num;i++){
         getline(&line, &len, fp);
         token=strtok(line,",");
         while(token!=NULL){
              var=atoi(token);
              printf("%d\n",var);
              token=strtok(NULL,",");
         }
    }
    fclose(fp);
    free(line);

    // int processes=0;
    // int workload[25]={0};
    // char time[25]={0};
    // char tickets[25]={0};
    //
    // char buf[1024];
    // int row_count = 0;
    // int field_count = 0;
    // while (fgets(buf, 1024, fp)) {
    //     field_count = 0;
    //     row_count++;
    //     //printf("test: %d", row_count);
    //
    //     if (row_count == 0) {
    //         continue;
    //     }
    //
    //     char *field = strtok(buf, ",");
    //     int i=0;
    //     int i2=0;
    //     int i3=0;
    //     //int dummy1=0;
    //     while (field) {
    //         if ((row_count == 1) && (field_count == 0)){
    //           //printf("Algoritmo: %s\n", field);
    //           //algoritmo = (char)*field;
    //           printf("Algoritmo: %s\n", field);
    //         }
    //         if ((row_count == 2) && (field_count == 0)){
    //           printf("Quatum: %s\n", field);
    //         }
    //         if ((row_count == 3) && (field_count == 0)){
    //           processes = (int)*field-48;
    //           printf("Procesos: %d\n", processes);
    //         }
    //         if (row_count == 4) {
    //           workload[i] = atoi(field);
    //           printf("Workload %d\n", workload[i]);
    //           i=i+1;
    //         }
    //         if (row_count == 5) {
    //           time[i2] = atoi(field);
    //           printf("Time %d\n", time[i2]);
    //           i2=i2+1;
    //         }
    //         if (row_count == 6) {
    //           tickets[i3] = atoi(field);
    //           printf("tickets %d\n", tickets[i3]);
    //           i3=i3+1;
    //         }
    //         //printf("%s\n", field);
    //         field = strtok(NULL, ",");
    //
    //         field_count++;
    //     }
    //     //printf("\n");
    // }
    //
    // fclose(fp);

    return 0;
}

//gcc CSVReader.c -g -Wall -o TestIOFile
