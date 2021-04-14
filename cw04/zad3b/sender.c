#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

int wait_flag = 0;
int received_counter = 0;
int signal_number = 0;
pid_t catcher_pid = 0;
char *mode = NULL;


void handler_flag() {
    wait_flag = 0;
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

    if (strcmp(mode, "kill_mode") == 0 || strcmp(mode, "queue_mode") == 0) {

        struct sigaction catcher_act1;
        catcher_act1.sa_flags = SA_SIGINFO;
        catcher_act1.sa_sigaction = handler_flag;
        sigaction(SIGUSR1, &catcher_act1, NULL);
        sigaction(SIGUSR2, &catcher_act1, NULL);

        if (strcmp(mode, "kill_mode") == 0) {

            for (int i = 0; i < signal_number; ++i) {
                while (wait_flag == 1) {}
                wait_flag = 1;
                kill(catcher_pid, SIGUSR1);
            }
            while (wait_flag == 1) {}
            kill(catcher_pid, SIGUSR2); //signal to end receiving
        } else {
            for (int i = 0; i < signal_number; i++) {
                while (wait_flag == 1) {}
                union sigval val;
                val.sival_ptr = NULL;
                val.sival_int = i;
                sigqueue(catcher_pid, SIGUSR1, val);
            }
            while (wait_flag == 1) {}
            union sigval val;
            sigqueue(catcher_pid, SIGUSR2, val);
            while (wait_flag == 1) {}
        }


        printf("wyslalem wszystko \n");

    } else if (strcmp(mode, "sigrt_mode") == 0) {

        struct sigaction catcher_act1;
        catcher_act1.sa_flags = SA_SIGINFO;
        catcher_act1.sa_sigaction = handler_flag;
        sigaction(SIGRTMIN + 1, &catcher_act1, NULL);
        sigaction(SIGRTMIN + 2, &catcher_act1, NULL);

        for (int i = 0; i < signal_number; i++) {
            while (wait_flag == 1) {}
            wait_flag = 1;
            kill(catcher_pid, SIGRTMIN + 1);
        }
        while (wait_flag == 1) {}
        kill(catcher_pid, SIGRTMIN + 2); //signal to end receiving
        printf("wyslalem wszystko \n");
    }
}



