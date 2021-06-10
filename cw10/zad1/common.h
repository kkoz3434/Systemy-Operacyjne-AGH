#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>

#define CLIENTS_N 6
#define MSG_LEN 16

int only_X_O(char msg[MSG_LEN]) {
    for (int i = 0; i < 10; ++i) {
        if (msg[i] != 'X' && msg[i] != 'O' && msg[i] != ' ') {
            return -1;
        }
    }
    return 1;
}
