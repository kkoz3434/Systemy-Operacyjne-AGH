#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include "common.h"

typedef struct client {
    int self;
    int friend;
} client;

int s_queue;
client clients_list[MAX_CLIENTS];
int clients_number = 0;

void exit_handler() {
    //delete queue
    printf("Deleting queue \n");
    msgctl(s_queue, IPC_RMID, NULL);
    exit(1);
}

void disconnect(msg *msg) {
    // disconnect chat
    int sender = msg->m_id;
    int partner = clients_list[sender].friend;
    if (partner != AVAILABLE) {
        clients_list[sender].friend = AVAILABLE;
        clients_list[partner].friend = AVAILABLE;

        struct msg disconnect_msg;
        disconnect_msg.m_id = s_queue;
        strcpy(disconnect_msg.string, "Chat has been closed\n");
        disconnect_msg.type = DISCONNECT;
        send_msg(clients_list[sender].self, &disconnect_msg);
        send_msg(clients_list[partner].self, &disconnect_msg);
    }
}

void stop_client(msg *msg) {
    int id = msg->m_id;
    // if we have partner to chat with:
    if (clients_list[id].friend != AVAILABLE)
        disconnect(msg);

    // update globals
    clients_list[id].self = -1;
    clients_number--;
    printf("Stopped client: %d\n", id);
}

void new_client(msg *msg) {
    //adding new clinet
    int new_id = -1;
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        if (clients_list[i].self == -1) {
            new_id = i;
            break;
        }
    }
    if (new_id == -1) {
        printf("No empty slots for client! \n");
        return;
    }
    //setting new client in array
    clients_number++;
    clients_list[new_id].self = msg->m_id;
    clients_list[new_id].friend = AVAILABLE;

    struct msg init_feedback;
    init_feedback.type = INIT;
    init_feedback.m_id = new_id;
    send_msg(clients_list[new_id].self, &init_feedback);
    printf("SENDED MSG TO: %d m_id: %d \n", clients_list[new_id].self, new_id);
    printf("Server confirmed adding new client: %d\n", new_id);
}

void connect_clients(msg *msg) {
    int sender_id = msg->m_id;
    int partner_id = atoi(msg->string);

    printf("CONNECT %d, %d \n", sender_id, partner_id);

    //if partner exists and is available
    if (clients_list[partner_id].self != -1 && clients_list[partner_id].friend == AVAILABLE) {
        printf("CONNECTIONG TO CHAT: %d with %d\n", sender_id, partner_id);
        printf("Sender queue: %d, Partner queue: %d\n", clients_list[sender_id].self, clients_list[partner_id].self);

        // make messages for sender and partner
        struct msg msg_sender;
        msg_sender.type = CONNECT;
        msg_sender.m_id = s_queue;
        // convert int to string
        char s1[MSG_SIZE];
        sprintf(s1, "%d", clients_list[partner_id].self);
        printf("Message for sender: %s", s1);
        strcpy(msg_sender.string, s1);

        struct msg msg_partner;
        msg_partner.type = CONNECT;
        msg_partner.m_id = s_queue;
        char s2[MSG_SIZE];
        sprintf(s2, "%d", clients_list[sender_id].self);
        printf("Message for partner: %s", s2);
        strcpy(msg_partner.string, s2);

        // set connection in array
        clients_list[sender_id].friend = partner_id;
        clients_list[partner_id].friend = sender_id;

        // send messages to clients_list
        send_msg(clients_list[sender_id].self, &msg_sender);
        send_msg(clients_list[partner_id].self, &msg_partner);
        printf("Successfully connect client %d with client %d\n", sender_id, partner_id);

    } else {
        printf("Requested client not available! \n");
        struct msg fail_respond;
        fail_respond.m_id = s_queue;
        fail_respond.type = CONNECT;
        strcat(fail_respond.string, "-1");
        send_msg(clients_list[sender_id].self, &fail_respond);
    }

}

void list_clients(msg *msg) {
    printf("Available clients_list to connect: \n");
    struct msg list_feedback;
    list_feedback.type = LIST;
    list_feedback.m_id = s_queue;

    for (int i = 0; i < MAX_CLIENTS; ++i) {
        // if client exist and is available
        if (clients_list[i].self != -1 && clients_list[i].friend == AVAILABLE) {
            printf("Client %d\n", i);
            char tmp[50];
            sprintf(tmp, "User %d is available\n", i);
            strcat(list_feedback.string, tmp);
        }
    }

    send_msg(clients_list[msg->m_id].self, &list_feedback);
    printf("Send LIST arr to %d client with queue m_id: %d\n", msg->m_id, clients_list[msg->m_id].self);
    printf("Server just sent clients_list list info\n");
}

int main() {
    // starting the server
    atexit(exit_handler);
    signal(SIGINT, exit_handler);

    // initializing the server
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients_list[i].self = -1;
        clients_list[i].friend = AVAILABLE;
    }

    key_t s_key = server_key();
    printf("Server key: %d\n", s_key);

    //creating the queue
    s_queue = msgget(s_key, IPC_CREAT | 0666);
    printf("Server queue at server.c :  %d\n", s_queue);

    // time to get some messages
    msg received;
    printf("WHILE(1) BEGINS\n");
    while (1) {
        if (msgrcv(s_queue, &received, MSG_SIZE, -100, IPC_NOWAIT) > 0) {
            switch (received.type) {
                case STOP:
                    stop_client(&received);
                    break;
                case DISCONNECT:
                    disconnect(&received);
                    break;
                case INIT:
                    new_client(&received);
                    break;
                case CONNECT:
                    printf("Connect starts!\n");
                    connect_clients(&received);
                    break;
                case LIST:
                    list_clients(&received);
                    break;
                default:
                    break;
            }
        }
    }
}