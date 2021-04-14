#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>


int wait_flag = 0;
char *mode = NULL;
int received_counter = 0;


void handler_USR1_kill_queue(int sig, siginfo_t *info, void *ucontext) {
    received_counter++;
    printf("catcher handler 1\n");
    kill(info->si_pid, SIGUSR1);
}
void handler_USR1_sigrt(int sig, siginfo_t *info, void *ucontext) {
    received_counter++;
    printf("catcher handler 1\n");
    kill(info->si_pid, SIGRTMIN + 1);
}



void handler_USR2(int sig, siginfo_t *info, void *ucontext) {
    printf("Received %d signals from Sender\n", received_counter);
    exit(0);
}

int main(int argc, char **argv) {
    printf("Catcher PID: %d\n", getpid());

    if (argc != 2) {
        perror("Wrong amount of arguments\n");
        return -100;
    }
    mode = argv[1];


    if (strcmp(mode, "kill_mode") == 0 || strcmp(mode, "queue_mode") == 0) {

        struct sigaction catcher_act1;
        catcher_act1.sa_flags = SA_SIGINFO;
        catcher_act1.sa_sigaction = handler_USR1_kill_queue;
        sigaction(SIGUSR1, &catcher_act1, NULL);

        struct sigaction catcher_act2;
        catcher_act2.sa_flags = SA_SIGINFO;
        catcher_act2.sa_sigaction = handler_USR2;
        sigaction(SIGUSR2, &catcher_act2, NULL);

        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGUSR1);
        sigdelset(&mask, SIGUSR2);
        sigprocmask(SIG_SETMASK, &mask, NULL);


        while (1) {
            sigsuspend(&mask);
        }
    } else if (strcmp(mode, "sigrt_mode") == 0) {
        struct sigaction catcher_act1;
        catcher_act1.sa_flags = SA_SIGINFO;
        catcher_act1.sa_sigaction = handler_USR1_sigrt;
        sigaction(SIGRTMIN + 1, &catcher_act1, NULL);

        struct sigaction catcher_act2;
        catcher_act2.sa_flags = SA_SIGINFO;
        catcher_act2.sa_sigaction = handler_USR2;
        sigaction(SIGRTMIN + 2, &catcher_act2, NULL);

        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGRTMIN + 1);
        sigdelset(&mask, SIGRTMIN + 2);
        sigprocmask(SIG_SETMASK, &mask, NULL);

        while (1) {
            sigsuspend(&mask);
        }
    }
}