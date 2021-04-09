#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <wait.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Need argument n: number of forks()");
        return 1;
    }

    pid_t child;

    int n = atoi(argv[1]);
    for (int j = 0; j < n; ++j) {
        child = fork();
        if (child == 0) {
            printf("Child printf from pid: %d, parent pid: %d\n", getpid(), getppid());
            exit(0);
        }
    }
    while (child = wait(NULL)) { // just infinite loop
        if (errno == ECHILD) { //ECHILD No child processes (POSIX.1-2001).
           // printf("just control print");
            return 0;
        }
    }
}
