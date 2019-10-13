#include <stdio.h>
#include <string.h>

/*
------- CSV FIle Organization ------
1)  Workload
1)  arrival time
1)  tickets
1)  quantum
1)  scheduler
*/

int main(void) {
    FILE *fp = fopen("sample.csv", "r");

    if (!fp) {
        printf("Can't open file\n");
        return 0;
    }

    char buf[1024];
    int row_count = 0;
    int field_count = 0;
    while (fgets(buf, 1024, fp)) {
        field_count = 0;
        row_count++;

        if (row_count == 0) {
            continue;
        }

        char *field = strtok(buf, ",");
        while (field) {
            if (field_count == 0) {
                printf("Workload:\t");
            }
            if (field_count == 1) {
                printf("Arrival Time:\t");
            }
            if (field_count == 2) {
                printf("Tickets:\t");
            }
            if (field_count == 3) {
                printf("Quatum:\t");
            }
            if (field_count == 4) {
                printf("Scheduler:\t");
            }
            printf("%s\n", field);
            field = strtok(NULL, ",");

            field_count++;
        }
        printf("\n");
    }

    fclose(fp);

    return 0;
}

//gcc CSVwritter.c -g -Wall -o TestIOFile
