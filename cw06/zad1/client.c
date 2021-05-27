#include <sys/ipc.h>d
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "common.h"

int s_queue = -1;
int on_server_id = -1;
int my_queue = -1;
int partner_queue = -1;

void exit_handler() {
    // we have to inform server about our disconnect
    if (s_queue >= 0 && on_server_id >= 0) {
        struct msg msg;
        msg.type = STOP;
        msg.m_id = on_server_id;
        send_msg(s_queue, &msg);
    }
    // delete our queue
    msgctl(my_queue, IPC_RMID, NULL);
    exit(1);
}

int main(int argc, char **argv) {
    // declare what happens when program is stopped
    atexit(exit_handler);

    signal(SIGINT, exit_handler);

    //now we have to init the client
    key_t my_key= client_key();

    // create queue
    my_queue = msgget(my_key, IPC_CREAT | 0666); //read write to all
    printf("My queue: %d", my_queue);

    // get server key and queue by proj_id
    key_t s_key= server_key();
    printf("Server key: %d\n", s_key);


    // msgget returns queue m_id if msgflg=0 and queue exists (manual)
    s_queue = msgget(s_key, 0);
    printf("Server queue: %d\n", s_queue);
    //time to inform server about our being

    struct msg init_message;
    init_message.m_id = my_queue;
    init_message.type = INIT;
    send_msg(s_queue, &init_message);
    printf("Sending queue to the server: %d\n", my_queue);

    fd_set fd_read, fd_save;
    FD_ZERO(&fd_read); //make empty fd_read
    FD_SET(STDIN_FILENO, &fd_read);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    printf("While(1) begins\n");
    fd_save = fd_read;

    int s;
    char command[30];
    while (1) {
        struct msg info;
        //If msgtyp is less than 0, then the first message in the queue with the lowest type less than or equal to the absolute value of msgtyp will be read. (manual)
        if (msgrcv(my_queue, &info, MSG_SIZE, -10, IPC_NOWAIT)> 0) { // get highest priority
            printf("waiting for message2\n");
            printf("Received type: %d\n", info.type);
            switch (info.type) {
                case STOP:
                    exit_handler();
                    break;
                case DISCONNECT:
                    partner_queue = -1;
                    printf("Disconnected from chat\n");
                    break;

                case CONNECT:
                    partner_queue = atoi(info.string);
                    printf("Connected with chat-partner: %d\n", partner_queue);
                    break;

                case LIST:
                    printf("%s", info.string);
                    break;

                case MSG:
                    printf("Message from %d client!\n", info.m_id);
                    printf("%s\n", info.string);
                    break;

                case INIT:
                    // receive msg from server to set global value
                    on_server_id = info.m_id;
                    printf("Received on_server_id: %d\n", on_server_id);
                default:
                    break;
            }
        }

        // fd_read the command from terminal -> until end
        // use select to block input until it appears
        //https://www.gnu.org/software/libc/manual/html_node/Waiting-for-I_002fO.html#:~:text=The%20fd_set%20data%20type%20represents,is%20actually%20a%20bit%20array.&text=The%20value%20of%20this%20macro,is%20at%20least%20that%20number.

        s = select(8, &fd_read, NULL, NULL, &timeout);
        if (s > 0) {
            //input is ready to fd_read
            int length;
            length = read(0, command, 30);
            command[length] = '\0';

            //time to exec our command
            char *parsed = strtok(command, " \n\0");
            printf("PRZECZYTAŁEM: %s\n", parsed);
            struct msg tmp;
            if (strcmp(parsed, "STOP") == 0) {
                exit_handler();

            } else if (strcmp(parsed, "DISCONNECT") == 0) {
                tmp.type = DISCONNECT;
                tmp.m_id = on_server_id;
                send_msg(s_queue, &tmp);

            } else if (strcmp(parsed, "CONNECT") == 0) {
                // parse partner on_server_id
                char *partner = strtok(NULL, " \n\0");
                printf("%s\n",partner);
                tmp.type = CONNECT;
                strcpy(tmp.string, partner);
                tmp.m_id = on_server_id;
                send_msg(s_queue, &tmp);

            } else if (strcmp(parsed, "LIST") == 0) {
                tmp.type = LIST;
                tmp.m_id = on_server_id;
                send_msg(s_queue, &tmp);

            } else if (strcmp(parsed, "MSG") == 0) {
                tmp.type=MSG;
                char *text_message = strtok(NULL, "\0");
                printf("To send: %s\n", text_message);
                strcpy(tmp.string, text_message);
                send_msg(partner_queue, &tmp);

            }
        }
        fd_read = fd_save;
    }

}












