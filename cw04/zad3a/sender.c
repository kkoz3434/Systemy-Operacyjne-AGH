#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

int received_counter = 0;
int signal_number = 0;
pid_t catcher_pid = 0;
char *mode = NULL;

void handler_USR1(int sig, siginfo_t *info, void *ucontext) {
    printf("sender handler 1\n");
    received_counter++; //increase amount of received
}
void handler_USR1_queue(int sig, siginfo_t *info, void *ucontext) {
    printf("sender handler 1\n");
    received_counter++; //increase amount of received
    printf("Signal from catcher is: %d\n", info->si_value.sival_int);

}

void handler_USR2(int sig, siginfo_t *info, void *ucontext) {
    printf("Signal transmission has ended! Received SIGUSR2 from:  \n", info->si_pid);
    printf("Send back handler received: %d, from %d sended\n", received_counter, signal_number);
    exit(0);
}


int main(int argc, char **argv) {
    printf("Starting sender \n");
    if (argc != 3) {
        printf("Incorrect amount of arguments\n");
        //return -1;
    }

    mode = argv[1];
    signal_number = atoi(argv[2]);


    char line[100];
    FILE *command = popen("pidof catcher", "r");
    fgets(line, 100, command);
    catcher_pid = strtoul(line, NULL, 10);
    pclose(command);


    printf("Signals to send: %d, to procces: %d\n", signal_number, catcher_pid);

    if (strcmp(mode, "kill_mode") == 0 || strcmp(mode, "queue_mode")==0) {

        struct sigaction catcher_act1;
        catcher_act1.sa_flags = SA_SIGINFO;
        if(strcmp(mode, "queue_mode")==0)
            catcher_act1.sa_sigaction = handler_USR1_queue;
        else
            catcher_act1.sa_sigaction = handler_USR1;
        sigaction(SIGUSR1, &catcher_act1, NULL);

        struct sigaction catcher_act2;
        catcher_act2.sa_flags = SA_SIGINFO;
        catcher_act2.sa_sigaction = handler_USR2;
        sigaction(SIGUSR2, &catcher_act2, NULL);

        if(strcmp(mode, "kill_mode") == 0) {
            for (int i = 0; i < signal_number; i++) {
                kill(catcher_pid, SIGUSR1);
            }
            kill(catcher_pid, SIGUSR2); //signal to end receiving
        }else{
            for (int i = 0; i < signal_number; i++) {
                union sigval val;
                val.sival_ptr= NULL;
                val.sival_int = i;
                sigqueue(catcher_pid, SIGUSR1, val);
            }
            union sigval val;
            sigqueue(catcher_pid, SIGUSR2, val); //signal to end receiving
        }

        printf("wyslalem wszystko \n");
        while (1) {}
    } else if (strcmp(mode, "sigrt_mode") == 0) {

        struct sigaction catcher_act1;
        catcher_act1.sa_flags = SA_SIGINFO;
        catcher_act1.sa_sigaction = handler_USR1;
        sigaction(SIGRTMIN + 1, &catcher_act1, NULL);

        struct sigaction catcher_act2;
        catcher_act2.sa_flags = SA_SIGINFO;
        catcher_act2.sa_sigaction = handler_USR2;
        sigaction(SIGRTMIN + 2, &catcher_act2, NULL);
        for (int i = 0; i < signal_number; i++) {
            kill(catcher_pid, SIGRTMIN + 1);
        }
        kill(catcher_pid, SIGRTMIN + 2); //signal to end receiving
        printf("wyslalem wszystko \n");
        while (1) {}
    }
}



