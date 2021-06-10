#include "common.h"

int socket_server;
char *name;
char my_sign;
char last_move;

void empty_board(char board[10]) {
    for (int i = 0; i < 10; ++i) {
        board[i] = ' ';
    }
}

int is_winning_msg(char board[10]) {
    if (board[0] == 'X' || board[0] == 'Y') {
        for (int i = 1; i < 10; ++i) {
            if (board[i] != 'E') {
                return -1;
            }
        }
        return 1;
    }
    return -1;
}

void take_move(char msg[MSG_LEN]) {
    printf("Choose your next field: ");
    int field;
    scanf("%d", &field);
    char msg2[MSG_LEN];
    if (msg[field] != ' ') {
        printf("Can't you count to 9? srsly? YOU LOSE  \n");
        strcpy(msg2, "L");
        send(socket_server, msg2, MSG_LEN, 0);
        exit(0);
    } else {
        msg[0] = my_sign;
        msg[field] = my_sign;
        send(socket_server, msg, MSG_LEN, 0);
    }
}

void display_board(char msg[MSG_LEN]) {
    printf("GOT BOARD: [%s] : %d\n", msg, strlen(msg));
    printf("#############\n");
    printf("%c | %c | %c \n", msg[1], msg[2], msg[3]);
    printf("%c | %c | %c \n", msg[4], msg[5], msg[6]);
    printf("%c | %c | %c \n", msg[7], msg[8], msg[9]);
    printf("#############\n");
}

void init(char *connection_type, char *network_address) {
    if (strcmp(connection_type, "local") == 0) {
        if ((socket_server = socket(AF_UNIX, SOCK_STREAM, 0)) <= 0) {
            printf("Socket error \n");
            printf("Error code: %d\n", errno);
            exit(1);
        }
        struct sockaddr_un s_un;
        s_un.sun_family = AF_UNIX;
        strcpy(s_un.sun_path, network_address);
        if (connect(socket_server, (struct sockaddr *) &s_un, sizeof(s_un)) < 0) {
            printf("Connect error \n");
            printf("Error code: %d\n", errno);
            exit(1);
        }
    } else if (strcmp(connection_type, "network") == 0) {
        if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
            printf("Socket error (network)\n");
            printf("Error code: %d\n", errno);
            exit(1);
        }
        struct sockaddr_in s_in;
        s_in.sin_family = AF_INET;
        s_in.sin_port = htons(atoi(network_address));
        s_in.sin_addr.s_addr = INADDR_ANY;
        if (connect(socket_server, (struct sockaddr *) &s_in, sizeof(s_in)) < 0) {
            printf("Connect error (network) \n");
            printf("Error code: %d\n", errno);
            exit(1);
        }
    } else {
        printf("Wrong connection types!\n");
        exit(1);
    }
}

void send_name() {
    char name_msg[MSG_LEN];
    sprintf(name_msg, "%s", name);
    send(socket_server, name_msg, MSG_LEN, 0);
    printf("Name to server send \n");
}

int main(int argc, char **argv) {
    if (argc != 4) {
        puts("wrong number of args \n");
        exit(1);
    }
    name = argv[1];
    init(argv[2], argv[3]);
    printf("Client works! \n");
    printf("SOCKET: %d\n", socket_server);
    /// sending our name to server

    send_name();


    while (1) {
        char msg[MSG_LEN];

        recv(socket_server, msg, MSG_LEN, 0);
        if (strcmp(msg, "P") == 0) {
            //printf("PONG\n");
            send(socket_server, "P", MSG_LEN, 0);

        } else if (strcmp(msg, "NT") == 0) {
            printf("Name already exist\n");
            exit(0);

        } else if (strcmp(msg, "NO") == 0) {
            printf("Waiting for 2nd player\n");

        } else if (strcmp(msg, "OSIGN") == 0) {
            my_sign = 'O';
            printf("My sign: %c\n", my_sign);

        } else if (strcmp(msg, "W") == 0) {
            printf("Congrats! You won! \n");
            exit(0);

        } else if (strcmp(msg, "XSIGN") == 0) {
            my_sign = 'X';
            printf("My sign: %c\n", my_sign);
            printf("STARTING GAME... MY GOD, FINALLY! \n");
            empty_board(msg);
            display_board(msg);
            take_move(msg);

        } else if (only_X_O(msg)) {
            last_move = msg[0];
            if (is_winning_msg(msg) == 1) {
                break;
            }
            display_board(msg);
            take_move(msg);
        }
    }
    if (last_move == my_sign) {
        printf("You won! \n");
    } else {
        printf("You loose!\n");
    }
}


