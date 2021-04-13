#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#define MY_SIGNAL SIGUSR2


void handler_usr2() {
    printf("I'm handling USR2 signal\n");
}

void ignore_fork() {
    signal(MY_SIGNAL, SIG_IGN); // SIG_IGN ignores the signal, do nothing
    pid_t child = fork();
    if (child == 0) {
        raise(MY_SIGNAL); // <=> kill (getppid(), sig);
        exit(0);
    }
    while (child = wait(NULL)) {
        if (errno == ECHILD) {
            printf("Ignore succeds\n");
            break;
        }
    }
}

void ignore_exec() {
    signal(MY_SIGNAL, SIG_IGN); // SIG_IGN ignores the signal, do nothing
    raise(MY_SIGNAL);
    execlp("./child", "./child", "ignore", NULL);
    perror("Return not expected. Must be an execl() error.n\n"); // line from man :D
}

void handle() {
    signal(MY_SIGNAL, handler_usr2);
    pid_t child = fork();
    if (child == 0) {
        raise(MY_SIGNAL);
        exit(0);
    }
    while (child = wait(NULL)) {
        if (errno == ECHILD) {
            printf("Handle succeds\n");
            break;
        }
    }
}

void mask_fork() {
    sigset_t new_mask, old_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, MY_SIGNAL);

    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) { // set new mask
        perror("Can't block MY_SIGNAL\n");
    }

    raise(MY_SIGNAL); // we are sending signal to ourselves

    sigset_t tmp;
    sigemptyset(&tmp);
    sigpending(&tmp); // get the pending signals

    if (sigismember(&tmp, MY_SIGNAL) == 1)
        printf("MY_SIGNAL is in set!\n");
    else
        perror("MY_SIGNAL is #NOT# in set\n"); //check if signal is in mask


    pid_t child = fork();
    if (child == 0) {
        raise(MY_SIGNAL);
        sigset_t childset;
        sigpending(&childset);
        if (sigismember(&childset, MY_SIGNAL) == 1)
            printf("MY_SIGNAL is in mask set!\n");
        else
            perror("MY_SIGNAL is #NOT# in mask set\n");
        exit(0);
    }
    while (child = wait(NULL)) {
        if (errno == ECHILD) {
            printf("Mask succed\n");
            if (sigprocmask(SIG_SETMASK, &new_mask, NULL) < 0) {
                perror("Can't reback old mask\n");
            }
            break;
        }
    }
}

void mask_exec() {
    sigset_t new_mask, old_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, MY_SIGNAL);

    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) { // set new mask
        perror("Can't block MY_SIGNAL\n");
    }

    raise(MY_SIGNAL); // we are sending signal to ourselves

    sigset_t tmp;
    sigemptyset(&tmp);
    sigpending(&tmp); // get the pending signals

    if (sigismember(&tmp, MY_SIGNAL) == 1)
        printf("MY_SIGNAL is in set!\n");
    else
        perror("MY_SIGNAL is #NOT# in set\n"); //check if signal is in mask

    execl("./child", "./child", "mask", NULL);
    perror("Return not expected. Must be an execl() error.n\n"); // line from man :D
}

void pending_fork() {
    sigset_t new_mask, old_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, MY_SIGNAL);

    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) { // set new mask
        perror("Can't block MY_SIGNAL\n");
    }

    raise(MY_SIGNAL); // we are sending signal to ourselves

    sigset_t tmp;
    sigemptyset(&tmp);
    sigpending(&tmp); // get the pending signals

    if (sigismember(&tmp, MY_SIGNAL) == 1)
        printf("MY_SIGNAL is in set!\n");
    else
        perror("MY_SIGNAL is #NOT# in set\n"); //check if signal is in mask

    pid_t child = fork();
    if (child == 0) {
        sigset_t childset;
        sigpending(&childset);
        if (sigismember(&childset, MY_SIGNAL) == 1)
            printf("MY_SIGNAL is in mask set!\n");
        else
            perror("MY_SIGNAL is #NOT# in mask set\n");
        exit(0);
    }
    while (child = wait(NULL)) {
        if (errno == ECHILD) {
            printf("Mask succed\n");
            if (sigprocmask(SIG_SETMASK, &new_mask, NULL) < 0) {
                perror("Can't reback old mask\n");
            }
            break;
        }
    }
}

void pending_exec() {
    sigset_t new_mask, old_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, MY_SIGNAL);

    if (sigprocmask(SIG_BLOCK, &new_mask, &old_mask) < 0) { // set new mask
        perror("Can't block MY_SIGNAL\n");
    }

    raise(MY_SIGNAL); // we are sending signal to ourselves

    sigset_t tmp;
    sigemptyset(&tmp);
    sigpending(&tmp); // get the pending signals

    if (sigismember(&tmp, MY_SIGNAL) == 1)
        printf("MY_SIGNAL is in set!\n");
    else
        perror("MY_SIGNAL is #NOT# in set\n"); //check if signal is in mask
    execl("./child", "./child", "mask", NULL);
    printf("Return not expected. Must be an execl() error.n\n"); // line from man :D
}

int main(int argc, char **argv) {
    int i = 1;
    while (i < argc) {
        printf("%s: \n", argv[i]);
        if (strcmp(argv[i], "ignore") == 0) {
            printf("FORK ignore:\n");
            ignore_fork();
            printf("\nEXEC ignore:\n");
            ignore_exec();

        } else if (strcmp(argv[i], "handle") == 0) {
            handle();
            printf("\n");
        } else if (strcmp(argv[i], "mask") == 0) {
            printf("FORK mask:\n");
            mask_fork();
            printf("\nEXEC mask:\n");
            mask_exec();

        } else if (strcmp(argv[i], "pending") == 0) {
            printf("FORK pending:\n");
            pending_fork();
            printf("\nEXEC pending:\n");
            pending_exec();

        }
        printf("\n##########################\n");
        i++;
    }
    return 0;
}