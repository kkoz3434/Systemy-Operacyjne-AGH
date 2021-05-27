//
// Created by kkoz34 on 12.05.2021.
//

#include "semafores.h"

// memory
common_helper *helper;


int t_entrance;
int t_free, t_taken;

int pizza_to_take;

void end_work() {
    disconnect_memory(helper, sizeof (common_helper));
    exit(0);
}

void start_work() {

    printf("Driver works %d\n", getpid());
    helper = create_common_memory("helper.txt", sizeof(common_helper));

    //get semafores
    t_entrance = open_existing_semafore("t_entrance.txt");
    t_taken = open_existing_semafore("t_taken.txt");
    t_free = open_existing_semafore("t_free.txt");


    //signal handler
    signal(SIGINT, end_work);

}

void work() {
    // wait for table and pizza to take
    sem_wait(t_taken);
    sem_wait(t_entrance);

    pizza_to_take = helper ->to_take_table;
    if (helper->table[pizza_to_take] == -1) {
        printf("something gone wrong with taking pizza from table\n");
    } else {
        printf("Driver %d took pizza %d from table!\n", getpid(), helper->table[pizza_to_take]);
        printf("Timestamp: %s\n", get_time());
    }
    helper->to_take_table = (helper->to_take_table+1)%TABLE_N;

    sem_post(t_entrance);
    sem_post(t_free);
}


int main() {
    start_work();
    while (1) {
        work();
    }

}