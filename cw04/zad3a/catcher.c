#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

char* mode =NULL;
int received_counter = 0;
void handler_USR1(){
    received_counter ++;
    printf("catcher handler 1\n");
}

void send_back(pid_t sender){
    if (strcmp(mode, "kill_mode")==0){
        for (int i = 0; i < received_counter; ++i) {
            kill(sender, SIGUSR1);
        }
        printf("Sended back received signals\n");
        kill(sender, SIGUSR2);
    }

    if (strcmp(mode, "sigrt_mode")==0){
        for (int i = 0; i < received_counter; ++i) {
            kill(sender, SIGRTMIN +1);
        }
        printf("Sended back received signals\n");
        kill(sender, SIGRTMIN +2);
    }

    if (strcmp(mode, "queue_mode")==0){
        for (int i = 0; i < received_counter; ++i) {
            union sigval val;
            val.sival_ptr= NULL;
            val.sival_int = i;
            sigqueue(sender, SIGUSR1, val);
        }
        printf("Sended back received signals\n");
        union sigval val;
        sigqueue(sender, SIGUSR2 , val);
    }
}

void handler_USR2(int sig, siginfo_t* info, void* ucontext){
    printf("Received %d signals from Sender\n", received_counter);
    send_back(info->si_pid);
    printf("Sended back catched signals, was PING time for PONG \n");
    exit(0);
}

int main(int argc, char** argv) {
printf("Catcher PID: %d\n", getpid());

if(argc!=2){
    perror("Wrong amount of arguments\n");
    return -100;
}
mode = argv[1];



if(strcmp(mode, "kill_mode")==0 || strcmp(mode, "queue_mode")==0){

    struct sigaction catcher_act1;
    catcher_act1.sa_flags= SA_SIGINFO;
    catcher_act1.sa_sigaction = handler_USR1;
    sigaction(SIGUSR1, &catcher_act1, NULL);

    struct sigaction catcher_act2;
    catcher_act2.sa_flags= SA_SIGINFO;
    catcher_act2.sa_sigaction = handler_USR2;
    sigaction(SIGUSR2, &catcher_act2, NULL);

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGUSR2);
    sigprocmask(SIG_SETMASK, &mask, NULL);


    while(1){
        sigsuspend(&mask);
    }
}
else if(strcmp(mode, "sigrt_mode")==0){
    struct sigaction catcher_act1;
    catcher_act1.sa_flags= SA_SIGINFO;
    catcher_act1.sa_sigaction = handler_USR1;
    sigaction(SIGRTMIN + 1, &catcher_act1, NULL);

    struct sigaction catcher_act2;
    catcher_act2.sa_flags= SA_SIGINFO;
    catcher_act2.sa_sigaction = handler_USR2;
    sigaction(SIGRTMIN + 2, &catcher_act2, NULL);

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGRTMIN + 1);
    sigdelset(&mask, SIGRTMIN + 2);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    while(1){
        sigsuspend(&mask);
    }
}
}