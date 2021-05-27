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
#include <time.h>
#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>

#include "semafores.h"


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

//// https://codereview.stackexchange.com/questions/11921/getting-current-time-with-milliseconds
char *get_time() {
    struct timeval time;
    gettimeofday(&time, NULL);
    int mili = time.tv_usec / 1000;
    char buff[80];
    strftime(buff, 80, "%Y-%m-%d %H:%M:%S", localtime(&time.tv_sec));

    char cbuff[84] = "";
    sprintf(cbuff, "%s:%03d", buff, mili);
    return cbuff;
}

int get_wait_seconds(int a, int b) {
    int r = (rand() % (b - a)) + a;
    return r;
}

struct common_helper *get_common_memory(char *file, int size) {
    int desc = shm_open(file, O_CREAT | O_RDWR, 0666);
    if (desc == -1) {
        perror("shm_open errror \n");
        exit(-1);
    }

    // map_shared specyfikacja uzycia segmentu
    common_helper *block = (common_helper *) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, desc, 0);
    return block;

}

struct common_helper *create_common_memory(char *file, int size) {
    if (size == 0) {
        perror("size must be >0\n");
        exit(-1);
    }
    // otwieramy deskryptor reprezentujacy segment pamieci wspolnej
    int desc = shm_open(file, O_CREAT | O_RDWR, 0666);
    if (desc == -1) {
        perror("shm_open errror \n");
        exit(-1);
    }

    // okrelamy jego rozmiar
    int trun = ftruncate(desc, size);
    if (trun == -1) {
        perror("ftruncate error\n");
        exit(-1);
    }

    //adres dołączonego segmentu
    common_helper *block = (common_helper *) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, desc, 0);
    return block;


}

void disconnect_memory(struct common_helper *mem, int size) {
    int mun = munmap(mem, size);
    if (mun == -1) {
        perror("munmap error \n");
        exit(-1);
    }
}

void delete_memory(char *file) {
    int unlink = shm_unlink(file);
    if (unlink == -1) {
        perror("shm_unlink error\n");
        exit(-1);
    }
}

sem_t *open_semafor(char *name, int init_val) {
    sem_t *sem =  sem_open(name, O_CREAT | O_RDWR, S_IRWXU, init_val);
    if((sem = sem_open(name, O_RDWR)) == -1){
        perror("sem_open new sem error\n");
        exit(-1);
    }
    return sem;
}

sem_t *open_existing_semafore(char *name) {
    sem_t *sem =  sem_open(name, O_RDWR);
    if((sem = sem_open(name, O_RDWR)) == -1){
        perror("sem_open existing sem error: %s\n");
        exit(-1);
    }
    return sem;
}


void close_semafor(char *name) {
    if (sem_close(name) == -1) {
        perror("sem_close error\n");
        exit(-1);
    }
}

void delete_semafor(char *name) {
    if (sem_unlink(name) == -1) {
        perror("sem_unlink error \n");
        exit(-1);
    }
}

#endif //ZAD1_SEMAFORS_H