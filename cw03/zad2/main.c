#include <stdio.h>
#include "lib.h"
#include <stdlib.h>
#include <sys/times.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int is_command(char *arg) {
    if (strcmp(arg, "merge_files") == 0)
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
        pid_t child;
        order = argv[i];

        if (strcmp(argv[i], "merge_files") == 0) {


            i++;
            while (i + 1 < argc && (is_command(argv[i]) == 0) && (is_command(argv[i + 1]) == 0)) {
                pid_t child = fork();
                if (child == 0) {
                    merge_two_tmp_file(argv[i], argv[i + 1]);
                    exit(0);
                }
                i += 2;
            }
            if (i < argc) {
                i++;
                if ((is_command(argv[i]) == 0)) {
                    pid_t child = fork();
                    if (child == 0) {
                        merge_two_tmp_file(argv[i], NULL);
                        exit(0);
                    }
                    i++;
                }
            }

        }

        i++;
        while(child = wait(NULL)){
            if(errno == ECHILD){
                break;
            }
        }
        end_time = times(end);

        printf("real time:  %lf\n", seconds(start_time, end_time));
        printf("user time:  %lf\n", seconds(start->tms_utime, end->tms_utime));
        printf(" sys time:  %lf\n", seconds(start->tms_stime, end->tms_stime));
        printf("\n");
        /*
        The <sys/times.h> header shall define the tms structure, which is returned by times() and shall include at least the following members:
        clock_t  tms_utime  User CPU time.
        clock_t  tms_stime  System CPU time.
        clock_t  tms_cutime User CPU time of terminated child processes.
        clock_t  tms_cstime System CPU time of terminated child processes.
         */


    }
}