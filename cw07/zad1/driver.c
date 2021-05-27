//
// Created by kkoz34 on 12.05.2021.
//

#include "semafors.h"

// memory
common_helper *helper;


int t_entrance;
int t_free, t_taken;

int pizza_to_take;

void end_work() {
    disconnect_memory(helper);
    exit(0);
}

void start_work() {

    printf("Driver works %d\n", getpid());
    helper = create_common_memory("helper.txt", TABLE_N);

    //get semafores
    t_entrance = get_semafor("t_entrance.txt");
    t_free = get_semafor("t_free.txt");
    t_taken = get_semafor("t_taken.txt");


    //signal handler
    signal(SIGINT, end_work);

}

void work() {
    // wait for table and pizza to take
    wait_semafor(t_taken);
    wait_semafor(t_entrance);
    int pizza;

    pizza_to_take = helper ->to_take_table;
    if (helper->table[pizza_to_take] == -1) {
        printf("something gone wrong with taking pizza from table\n");
    } else {
        printf("Driver %d took pizza %d from table!\n", getpid(), helper->table[pizza_to_take]);
        printf("Timestamp: %s\n", get_time());
        pizza = helper->table[pizza_to_take];
    }
    helper->to_take_table = (helper->to_take_table+1)%TABLE_N;

    post_semafor(t_entrance);
    post_semafor(t_free);
    sleep(get_wait_seconds(4,5));
    printf("Delivered pizza: $d, by %d\n", pizza, getpid());
    sleep(get_wait_seconds(4,5));
}


int main() {
    start_work();
    while (1) {
        work();
    }

}
