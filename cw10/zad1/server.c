#include "common.h"

struct Client {
    char name[256];
    int socket;
    int is_pinging;
    int enemy_socket;
};

int socket_local;
int socket_network;
struct Client clients[CLIENTS_N];
pthread_mutex_t server_mutex = PTHREAD_MUTEX_INITIALIZER;
int clients_n = 0;


void add_client(const char *msg, int client_fd) {
    clients[clients_n].socket = client_fd;
    strcpy(clients[clients_n].name, msg);
    clients[clients_n].is_pinging = 1;
    clients[clients_n].enemy_socket = -1;
    clients_n++;
    printf("CLIENT %s REGISTERED\n", msg);
}

void find_enemy(int client_fd) {
    int found = 0;
    for (int i = 0; i < clients_n; ++i) {
        if (clients[i].socket != client_fd && clients[i].enemy_socket == -1) {
            printf("Start pairing clients! \n");
            found = 1;
            clients[i].enemy_socket = client_fd;
            clients[clients_n - 1].enemy_socket = clients[i].socket;
            send(clients[clients_n - 1].socket, "XSIGN", MSG_LEN, 0);
            send(clients[i].socket, "OSIGN", MSG_LEN, 0);
            break;
        }
    }
    if (found == 0) {
        send(client_fd, "NO", MSG_LEN, 0);
    }
}

void P_handler(int client_fd) {
    printf("got pong signal\n");
    for (int i = 0; i < clients_n; ++i) {
        if (clients[i].socket == client_fd) {
            clients[i].is_pinging = 1;
        }
    }
}

void L_handler(int client_fd) {
    for (int i = 0; i < clients_n; ++i) {
        if (clients[i].socket == client_fd) {
            send(clients[i].enemy_socket, "W", MSG_LEN, 0);
        }
    }
}

int check_game_status(char board[10]) {
    char last = board[0];
    for (int i = 1; i < 4; ++i) {
        if(board[i] == last && board[i+1] == last && board[i+2] == last){
            return 1;
        }
        if(board[i] == last && board[i+3] == last && board[i+6] == last){
            return 1;
        }
    }
    if(board[1] == last && board[5] == last && board[9] == last ){
        return 1;
    }
    if(board[7] == last && board[5] == last && board[3] == last ){
        return 1;
    }
    return -1;
}

void make_winnig_msg(char msg[10]) {
    for (int i = 1; i < 10; ++i) {
        msg[i] = 'E';
    }
}

void game_handler(char *msg, int client_fd) {
    for (int i = 0; i < clients_n; ++i) {
        if (clients[i].socket == client_fd) {
            if (check_game_status(msg) == 1) {
                make_winnig_msg(msg);
                send(clients[i].enemy_socket, msg, MSG_LEN, 0);
                send(client_fd, msg, MSG_LEN, 0);
            } else {
                send(clients[i].enemy_socket, msg, MSG_LEN, 0);
            }
        }
    }
}

int check_if_exist(const char *msg, int client_fd) {
    int new_client = 0;
    for (int i = 0; i < clients_n; ++i) {
        if (strcmp(msg, clients[i].name) == 0) {
            new_client = 1;
            printf("Client name already taken: %s\n", msg);
            send(client_fd, "NAME_TAKEN", MSG_LEN, 0);
        } else if (clients[i].socket == client_fd) {
            new_client = 1;
            printf("Socket already exist\n");
        }
    }
    return new_client;
}

void ping_pinging() {
/// ping active users
    for (int i = 0; i < clients_n; ++i) {
        if (clients[i].is_pinging == -1) {
            continue;
        }
        send(clients[i].socket, "P", MSG_LEN, 0);
        printf("Name: %s\n", clients[i].name);
        clients[i].is_pinging = -1;

    }
}

void delete_non_pinging() {
/// remove not active clients
    for (int i = 0; i < clients_n; ++i) {
        if (clients[i].is_pinging == -1) {
            if (i < (clients_n - 1)) {
                printf("not responding: %s", clients[i].name);
                clients[i] = clients[clients_n - 1];
            }
            clients_n--;
        }
    }
}

void *ping_all() {
    while (1) {
        sleep(15);
        pthread_mutex_lock(&server_mutex);
        printf("PINGING... \n");

        delete_non_pinging();
        ping_pinging();

        pthread_mutex_unlock(&server_mutex);
    }
}

void init(char *pathname, int ip) {
    /// NETWORK SOCKET INIT
    if ((socket_network = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
        printf("Socket failed (network) \n");
        exit(1);
    }
    struct sockaddr_in s_in;
    s_in.sin_family = AF_INET;
    s_in.sin_port = htons(ip);
    s_in.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket_network, (struct sockaddr *) &s_in, sizeof(s_in)) < 0) {
        printf("Bind error (network)\n");
        printf("Error code: %d\n", errno);
        exit(1);
    }
    if (listen(socket_network, CLIENTS_N) < 0) {
        printf("Listen error (network)\n");
        printf("Error code: %d\n", errno);
        exit(1);
    }


    /// LOCAL SOCKET INIT
    if ((socket_local = socket(AF_UNIX, SOCK_STREAM, 0)) <= 0) {
        printf("Socket failed\n");
        exit(1);
    }

    struct sockaddr_un s_un;
    s_un.sun_family = AF_UNIX;
    strcpy(s_un.sun_path, pathname);
    unlink(pathname);

    if (bind(socket_local, (struct sockaddr *) &s_un, sizeof(s_un)) < 0) {
        printf("Bind error \n");
        printf("Error code: %d\n", errno);
        exit(1);
    }
    if (listen(socket_local, CLIENTS_N) < 0) {
        printf("Listen error \n");
        printf("Error code: %d\n", errno);
        exit(1);
    }
}

int find_client() {
    /// function gets the right socket to work with later

    pthread_mutex_lock(&server_mutex);
    /// define array of pollfd structs[CLIENT_N + 2], bc we have 2 sockets more
    struct pollfd *s_fds = calloc(2 + CLIENTS_N, sizeof(struct pollfd));

    ///firstly: our server sockets
    s_fds[0].fd = socket_local;
    s_fds[1].fd = socket_network;
    s_fds[0].events = POLLIN;
    s_fds[1].events = POLLIN;

    for (int i = 0; i < CLIENTS_N; ++i) {
        s_fds[i + 2].fd = clients[i].socket;
        s_fds[i + 2].events = POLLIN;
    }
    pthread_mutex_unlock(&server_mutex);

    /// just wait for the event
    poll(s_fds, 2 + CLIENTS_N, -1); //-1 => infinity waiting


    for (int i = 0; i < 2 + CLIENTS_N; ++i) {
        if (s_fds[i].revents == POLLIN) {
            if (s_fds[i].fd == socket_local || s_fds[i].fd == socket_network) {
                printf("FOUND CLIENT N.O. %d\n", i);
                return accept(s_fds[i].fd, NULL, NULL);
            }
            return s_fds[i].fd;
        }
    }
    return -1;


}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Wrong amount of arguments! \n");
        exit(1);
    }

    init(argv[2], atoi(argv[1]));
    printf("Server starts: connections works! \n");
    printf("SOCKET: %d\n", socket_local);

    /// set pinging machine
    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, ping_all, NULL);

    ///while loop for the server (just beginning :)
    while (1) {
        char msg[MSG_LEN];

        /// time to find the clients
        int client_fd = find_client();

        /// time to get the msg
        if ((recv(client_fd, msg, MSG_LEN, 0) > 0) && client_fd != -1) {

            /// going to use common memory
            pthread_mutex_lock(&server_mutex);
            if (strcmp(msg, "P") == 0) {
                P_handler(client_fd);
            } else if (strcmp(msg, "L")==0) {
                L_handler(client_fd);
            } else if (only_X_O(msg) == 1) {
                game_handler(msg, client_fd);
            } else {
                printf("New name: %s\n", msg);
                int new_client = check_if_exist(msg, client_fd);

                if (new_client == 0) {
                    add_client(msg, client_fd);
                    find_enemy(client_fd);
                }
            }
            pthread_mutex_unlock(&server_mutex);
        }
    }
}




