//
// Created by kkoz34 on 12.05.2021.
//

#ifndef ZAD1_SEMAFORS_H
#define ZAD1_SEMAFORS_H

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <string.h>

#include "semafors.h"


#define TABLE_N 5
#define FURNANCE_N 5
#define PIZZA_N 10

typedef struct {
    int furnance[FURNANCE_N];
    int table[TABLE_N];
    int to_put_table;
    int to_put_furnance;
    int to_take_table;
    int to_take_furnance; // zbedne?
} common_helper;

/// https://man7.org/linux/man-pages/man2/semctl.2.html
union semun {
    int val;   /* Value for SETVAL */
    struct semid_ds *buf;   /* Buffer for IPC_STAT, IPC_SET */
    unsigned short *array;  /* Array for GETALL, SETALL */
    struct seminfo *__buf; /* Buffer for IPC_INFO (Linux-specific) */
};

//// https://codereview.stackexchange.com/questions/11921/getting-current-time-with-milliseconds
char* get_time(){
    struct timeval time;
    gettimeofday(&time, NULL);
    int mili = time.tv_usec/1000;
    char *buff=calloc(80, sizeof(char));
    strftime(buff, 80, "%Y-%m-%d %H:%M:%S", localtime(&time.tv_sec));

    char *cbuff=calloc(80, sizeof(char));
    sprintf(cbuff, "%s:%03d", buff, mili);
    return cbuff;

}

int get_wait_seconds(int a, int b){
    int r = (rand()%(b-a)) + a;
    return r;
}


int create_semafor(char *file, int start_value){
    key_t semafor_key;

    semafor_key = ftok(file, 0);
    if ( semafor_key == -1){
        perror("Ftok error\n");
        exit(-1);
    }

    int semafor_id = semget(semafor_key, 1, 0666 | IPC_CREAT);
    if ( semafor_id == -1){
        perror("Semget error\n");
        exit(-1);
    }
    union semun init;
    init.val = start_value;
    int i = semctl(semafor_id, 0 , SETVAL, init);
    if ( i == -1){
        perror("semctl reeor\n");
        exit(-1);
    }

    return semafor_id;

}

int get_semafor(char *file){
    key_t semafor_key;
    semafor_key = ftok(file, 0 );
    if (semafor_key == -1){
        perror("ftok error\n");
        exit(-1);
    }

    // nsems == 0 dla odczytu semafora
    int semafor_id = semget(semafor_key, 0 ,0);
    if(semafor_id==-1){
        perror("semget error\n");
        exit(-1);
    }

    return semafor_id;
}

void wait_semafor(int semafor_id){
    struct sembuf to_do;

    // zmiana w semaforze IO, odwolujemy sie do 0
    to_do.sem_num = 0;

    // sprobujemy odjac 1
    to_do.sem_op = -1;

    // jesli proces sie zakonczy cofamy operacje na semaforze
    to_do.sem_flg = SEM_UNDO;

    //czekanko

    if(semop(semafor_id, &to_do, 1 ) == -1){
        perror("semop error\n");
        exit(-1);
    }
}

void post_semafor(int semafor_id){

    struct sembuf to_do;
    to_do.sem_num = 0;
    to_do.sem_op = 1;
    to_do.sem_flg = SEM_UNDO;

    if(semop(semafor_id, &to_do, 1)==-1){
        perror("semop error\n");
        exit(-1);
    }

}

struct common_helper *create_common_memory(char *file, int size){
    if(size == 0){
        perror("size must be >0\n");
        exit(-1);
    }

    key_t key = ftok(file, 0);
    if(key == -1){
        perror("ftok error\n");
        exit(-1);
    }

    // tworzenie semgentu pamieci wspolnej size !=0
    int memory_block = shmget(key, size, 0666| IPC_CREAT);
    if(memory_block == -1){
        perror("shmget error\n");
        exit(-1);
    }

    //przypisanie do przesrzeni adresowej procesu
    // NULL as recommended, no extra flags => 0
    struct common_helper *block = shmat(memory_block, NULL, 0 );
    if (block ==- 1){
        perror("shmat error\n");
    }
    return block;
}

void disconnect_memory(struct common_helper *mem){
    int i = shmdt(mem);
    if(i == -1){
        perror("shmdt error\n");
        exit(-1);
    }
}

void delete_memory(char *file){
    key_t key = ftok(file,0);
    if(key == -1){
        perror("ftok error\n");
        exit(-1);
    }

    int memory_block = shmget(key, 0 , 0666 | IPC_CREAT);
    if(memory_block == -1){
        perror("shmget error\n");
        exit(-1);
    }

    int del = shmctl(memory_block, IPC_RMID, NULL);
    if (del==-1){
        perror("shmctl error\n");
        exit(-1);
    }
}

void delete_semafor(int semafor_id){
    if(semctl(semafor_id, 0 , IPC_RMID) == - 1){
        perror("semctl error\n");
        exit(-1);
    }
}
#endif //ZAD1_SEMAFORS_H
