#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#define MY_SIGNAL SIGUSR2


int main(int argc, char* argv[]){
    if(strcmp(argv[1], "ignore") == 0){
        raise(MY_SIGNAL);
        printf("EXEC: raising signal in child (ignore)!\n");
    }
    if(strcmp(argv[1], "mask")==0){
        raise(MY_SIGNAL);
        sigset_t tmp;
        sigpending(&tmp);

        if(sigismember(&tmp, MY_SIGNAL)==1){
            printf("EXEC: rRaised signal is in pending set(mask)\n");
        }
        else{
            perror("EXEC: raised signal is not in set (mask)\n");
        }
    }
    if(strcmp(argv[1], "pending")==0){
        sigset_t tmp;
        sigpending(&tmp);

        if(sigismember(&tmp, MY_SIGNAL)==1){
            printf("EXEC: raised signal is in pending set(mask)\n");
        }
        else{
            perror("EXEC: raised signal is not in set (mask)\n");
        }
    }
    exit(0);
}