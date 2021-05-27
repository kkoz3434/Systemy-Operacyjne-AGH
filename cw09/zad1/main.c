#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#define REINDEERS_N 9
#define ELVES_N 10
#define ELVES_WAITING_N 3
#define MAX_TRIPS 3

int elves_waiting = 0;
int elves_waiting_arr[ELVES_WAITING_N];
pthread_t elves_arr[ELVES_N];

int reindeers_waiting = 0;
int reindeers_waiting_arr[REINDEERS_N];
pthread_t reindeers_arr[REINDEERS_N];

int santa_trips = 0;

pthread_t santa_thread;

// mutexes
pthread_mutex_t elves_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeers_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;

//conditions for threads
pthread_cond_t empty_elves_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty_reindeers_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;


int get_wait_seconds(int a, int b) {
    int r = (rand() % (b - a)) + a;
    return r;
}

void clear_arr(int arr[], int arr_len) {
    for (int i = 0; i < arr_len; ++i) {
        arr[i] = -1;
    }
}

void clean(){
    pthread_mutex_lock(&elves_mutex);
    pthread_mutex_destroy(&elves_mutex);

    pthread_mutex_lock(&santa_mutex);
    pthread_mutex_destroy(&santa_mutex);

    pthread_mutex_lock(&reindeers_mutex);
    pthread_mutex_destroy(&reindeers_mutex);
    exit(0);
}

_Noreturn void *santa_life() {
    while (1) {

        pthread_mutex_lock(&santa_mutex);
        printf("Santa is sleeping: elves = %d, reindeers = %d\n", elves_waiting, reindeers_waiting);
        pthread_cond_wait(&santa_cond, &santa_mutex);

        pthread_mutex_unlock(&santa_mutex);
        printf("Santa is waking up... elves: %d, reindeers: %d\n", elves_waiting, reindeers_waiting);


        if(reindeers_waiting == REINDEERS_N){
            pthread_mutex_lock(&reindeers_mutex);
            printf("############  \n Santa is delivering the gifts!\n############ \n");
            reindeers_waiting=0;
            clear_arr(reindeers_waiting_arr, REINDEERS_N);
            santa_trips++;
            pthread_cond_broadcast(&empty_reindeers_cond);
            pthread_mutex_unlock(&reindeers_mutex);
        }



        if (elves_waiting == ELVES_WAITING_N) {
            pthread_mutex_lock(&elves_mutex);

            printf("Helping elves: %d | %d | %d\n", elves_waiting_arr[0], elves_waiting_arr[1], elves_waiting_arr[2]);

            sleep(get_wait_seconds(1, 2));
            elves_waiting = 0;
            clear_arr(elves_waiting_arr, ELVES_WAITING_N);
            pthread_cond_broadcast(&empty_elves_cond);
            pthread_mutex_unlock(&elves_mutex);
        }

        if(santa_trips == MAX_TRIPS){
            printf("Santa's mission completed\n");
            clean();
            break;
        }
    }
}

_Noreturn void *elf_life(void *args) {
    int my_id = *((int *) args);
    while (1) {
        sleep(get_wait_seconds(2, 5));
        printf("Elf no. %d ended work\n", my_id);

        pthread_mutex_lock(&elves_mutex);
        while (elves_waiting == ELVES_WAITING_N) {
            printf("Elf no. %d waiting in queue (%d)\n", my_id, elves_waiting);
            pthread_cond_wait(&empty_elves_cond, &elves_mutex);
        }

        if (elves_waiting == 2) {
            elves_waiting_arr[elves_waiting] = my_id;
            elves_waiting++;
            printf("Elf no. %d waking up santa\n", my_id);
            pthread_cond_broadcast(&santa_cond);
            pthread_cond_wait(&empty_elves_cond, &elves_mutex);
        } else if (elves_waiting < 2) {
            elves_waiting_arr[elves_waiting] = my_id;
            elves_waiting++;
            printf("Elf no. %d waiting in queue\n", my_id);
            pthread_cond_wait(&empty_elves_cond, &elves_mutex);
        }
        pthread_mutex_unlock(&elves_mutex);
        if(santa_trips == MAX_TRIPS){
            break;
        }

    }
}

_Noreturn void *reindeer_life(void *args) {
    int my_id = *((int *) args);
    while (1) {
        sleep(get_wait_seconds(5, 10));
        printf("Reindeer no. %d came back from holiday \n", my_id);

        pthread_mutex_lock(&reindeers_mutex);
        if (reindeers_waiting == REINDEERS_N - 1) {
            reindeers_waiting_arr[reindeers_waiting] = my_id;
            reindeers_waiting++;
            printf("Reindeer no. %d is waking up the santa\n", my_id);
            pthread_cond_broadcast(&empty_reindeers_cond);
        } else if (reindeers_waiting < REINDEERS_N) {
            printf("Reindeer no. %d is ready to go (%d)\n", my_id, reindeers_waiting);
            reindeers_waiting_arr[reindeers_waiting] = my_id;
            reindeers_waiting++;

        }
        pthread_mutex_unlock(&reindeers_mutex);
        if(santa_trips == MAX_TRIPS){
            break;
        }

    }

}


int main() {

    // time to create santa! hoo hoo hoo

    pthread_create(&santa_thread, NULL, &santa_life, NULL);

    for (int i = 0; i < ELVES_N; ++i) {
        elves_arr[i] = i;
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&elves_arr[i], NULL, &elf_life, arg);
    }

    for (int i = 0; i < REINDEERS_N; ++i) {
        reindeers_arr[i] = i;
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&reindeers_arr[i], NULL, &reindeer_life, arg);
    }


    // wait for threads
    for (int i = 0; i < ELVES_N; ++i) {
        pthread_join(elves_arr[i], NULL);
    }
    for (int i = 0; i < REINDEERS_N; ++i) {
        pthread_join(reindeers_arr[i], NULL);
    }
    pthread_join(santa_thread, NULL);

}

// https://stackoverflow.com/questions/19232957/pthread-create-and-passing-an-integer-as-the-last-argument









