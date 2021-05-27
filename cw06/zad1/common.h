
#ifndef CONST
#define CONST

#define SEED 's'
#define MAX_CLIENTS 10
#define AVAILABLE 1
#define NOT_AVAILABLE -1
#define MSG_SIZE 512


#define STOP 1
#define DISCONNECT 2
#define INIT 3
#define CONNECT 4
#define LIST 5
#define MSG 6


typedef struct msg {
    long type;
    int m_id;
    char string[MSG_SIZE];
} msg;


key_t server_key(){
    key_t key;
    if ((key = ftok(getenv("HOME"), SEED)) == -1){
        perror("Couldn't get server key\n");
    }
    return key;
}

key_t client_key(){
    key_t key;
    if ((key = ftok(getenv("HOME"), getpid())) == -1){
        perror("Couldn't get client key\n");
    }
    return key;
}

int send_msg(int queue, msg *msg) {
    if ((msgsnd(queue, msg, MSG_SIZE, 0)) == -1) {
        printf("Can't send msg! \n");
    }
}
#endif

