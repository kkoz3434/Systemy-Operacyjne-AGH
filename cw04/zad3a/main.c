#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

void handle_sigsegv(int sig, siginfo_t *info, void *context) {
    printf("\n\n Handling SIGSEGV \n");
    printf("Signal number: %d sent from procces: %d\n", sig, info->si_pid);
    printf("SIGSEGV on addres: %p \n", info->si_addr); // *si_addr; * Memory location which caused fault *
    printf("System time: %ld", info->si_utime); //clock_t  si_utime;     /* User time consumed */
    raise(SIGKILL); // abort infinity :)
}

void sigsegv_test() {
    pid_t child = fork();
    if (child == 0) {
        struct sigaction act;
        act.sa_sigaction = handle_sigsegv;
        act.sa_flags = SA_SIGINFO;
        sigset_t tmp;
        sigemptyset(&tmp);
        act.sa_mask = tmp;
        sigaction(SIGSEGV, &act, NULL); // set handler for signal,
        // NULL bc we don"t want to remember setup
        char *test[] = {'a', 'b'};
        printf("%c", test[50500]);
    }
    while (child = wait(NULL)) {
        if (errno == ECHILD) {
            break;
        }
    }
}

void handle_sigint(int sig, siginfo_t *info, void *context) {
    printf("just got %d signal from user %d\n", info->si_signo, info->si_uid);
    printf("time to end life of sigint_test() function, took: %f seconds \n", (double) (info->si_stime));
}

void sigint_test() {
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handle_sigint;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);
    printf("wait a sec..\n");
    sleep(2);
    raise(SIGINT);
}

void handle_sigchld(int sig, siginfo_t *info, void *context) {
    printf("just got %d signal: \n", info->si_signo);
    printf("end of child life with status: %d \n", info->si_status);
}

void sigchld_test() {
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = handle_sigchld;
    sigemptyset(&act.sa_mask);
    sigaction(SIGCHLD, &act, NULL);

    pid_t child = fork();
    if (child == 0) {
        exit(0);
    }
    while (child = wait(NULL)) {
        if (errno == ECHILD) {
            break;
        }
    }
}

void resethand_handler(int sig) {
    printf("I will appear only once :) R E S E T T I N G \n");
}

void resethand_test() {
    struct sigaction act;
    act.sa_flags = SA_RESETHAND;
    //Przy wejściu do procedury obsługi sygnału
    //jest przywracana domyślna (SIG_DFL)
    //dyspozycja dla tego sygnału. Ten typ działania
    //jest zgodny z funkcjonowaniem wcześniejszych,
    //zawodnych sygnałów.

    sigemptyset(&act.sa_mask);
    act.sa_handler = resethand_handler;
    sigaction(SIGUSR1, &act, NULL);
    pid_t child = fork();
    if (child == 0) {
        raise(SIGUSR1);
        raise(SIGUSR1);
        raise(SIGUSR1);
        raise(SIGUSR1);
        exit(0);
    }
    while (child = wait(NULL)) {
        if (errno == ECHILD) {
            break;
        }
    }
}

void nodefer_handler(int sig) {
    printf("Signal: %d in procces: %d\n", sig, getpid());
}

void nodefer_test() {
    struct sigaction act;
    act.sa_handler = nodefer_handler;
    act.sa_flags = SA_RESTART; // nie konczy programu jesli przerwiemy wprowadzanie
    sigaction(SIGINT, &act, NULL);
    printf("insert number: \n");
    int number;
    scanf("%d", &number);
    printf("You choosed %d \n", number);

}

int main(int argc, char **argv) {
    printf("SIGINFO tests \n");
    sigsegv_test();
    sigint_test();
    sigchld_test();
    printf("Other flags tests: \n");
    resethand_test();
    nodefer_test();


    return 0;
}
