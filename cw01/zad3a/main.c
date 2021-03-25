#include <stdio.h>
#include "lib.h"
#include <stdlib.h>
#include <sys/times.h>
#include <string.h>
#include <unistd.h>

int is_command(char *arg) {
    if ((strcmp(arg, "create_table") == 0) || (strcmp(arg, "merge_files") == 0) ||
        (strcmp(arg, "remove_block") == 0) || (strcmp(arg, "remove_row") == 0) ||
        (strcmp(arg, "merge_insert") == 0))
        return 1;
    else
        return 0;
}

double seconds(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

int main(int argc, char **argv) {

    struct tms *start = malloc(sizeof(struct tms));
    struct tms *end = malloc(sizeof(struct tms));

    char ***table;

    int table_size;

    clock_t start_time;
    clock_t end_time;
    char *order;
    //printf("%d\n", argc);
    int i = 1;
    //printf("\n %s \n", argv[50]);
    while (i < argc) {
        start_time = times(start);
        order = argv[i];
        //printf("%s", argv[i]);

        if (strcmp(argv[i], "create_table") == 0) {
            i++;
            table_size=atoi(argv[i]);
            table = create_table(atoi(argv[i]));
        } else if (strcmp(argv[i], "merge_files") == 0) {
            //printf("zaczynam merge_file_sequence");
            i++;
            while (i + 1 < argc && (is_command(argv[i]) == 0) && (is_command(argv[i + 1]) == 0)) {
                merge_two_tmp_file_void(argv[i], argv[i + 1]);
                i += 2;
            }
            if (i < argc) {
                i++;
                if ((is_command(argv[i]) == 0)) {
                    merge_two_tmp_file_void(argv[i], NULL);
                    i++;
                }
            }

        } else if (strcmp(argv[i], "merge_insert") == 0) {
            i++;
            int repeats= atoi(argv[i]);
            i++;
            char** file_name=calloc(sizeof(char *),repeats);
            int index=1;
            while(index<repeats) {
                strcpy(file_name[index], argv[i]);
                index++;
            }
            table=merge_all(repeats, file_name);

        } else if (strcmp(argv[i], "remove_row") == 0) {
            i++;
            int block_number = atoi(argv[i++]);
            int row_number = atoi(argv[i]);
            delete_line(table[block_number], row_number);
            i++;
        } else if (strcmp(argv[i], "remove_block") == 0) {
            i++;
            int block_amount = atoi(argv[i]);
            for (int j = 0; j < block_amount/2 ; ++j) {
                delete_pair(table, 0);
            }
            i++;
        }
        i++;
        //printf("i dla next: %d", i);
        end_time = times(end);

        printf("\n # %s #\n", order);
        printf("real time:  %lf\n", seconds(start_time, end_time));
        printf("user time:  %lf\n", seconds(start->tms_utime, end->tms_utime));
        printf(" sys time:  %lf\n", seconds(start->tms_stime, end->tms_stime));

        /*
        The <sys/times.h> header shall define the tms structure, which is returned by times() and shall include at least the following members:

        clock_t  tms_utime  User CPU time.
        clock_t  tms_stime  System CPU time.
        clock_t  tms_cutime User CPU time of terminated child processes.
        clock_t  tms_cstime System CPU time of terminated child processes.

         */


    }
}
