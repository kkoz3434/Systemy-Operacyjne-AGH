//
// Created by kkoz34 on 12.05.2021.
//

#include "semafors.h"

// numbers
int p_makers, drivers;

//childs array
int *p_maker_pids, *drivers_pids;

//memory
common_helper *helper;

// semafors
int f_entrance, t_entrance;
int f_free, f_taken, t_free, t_taken;


void close_pizzeria(){
    printf("TIME TO CLOSE! \n");
    for (int i = 0; i < p_makers; ++i) {
        kill(p_maker_pids[i] , SIGINT);
    }

    for (int i = 0; i < drivers; ++i) {
        kill(drivers_pids[i] , SIGINT);
    }

    //odlaczamy pamiec i usuwamy
    disconnect_memory(helper);
    delete_memory("f.txt");
    delete_memory("t.txt");


    // usuwamy semafory
    delete_semafor(f_entrance);
    delete_semafor(f_free);
    delete_semafor(f_taken);

    delete_semafor(t_entrance);
    delete_semafor(t_free);
    delete_semafor(t_taken);
}

void open_pizzeria() {
    // tworzenie pamieci wspolnej

    printf("PIZZERIA STARTS THE DAY!\n");

    helper = create_common_memory("helper.txt", sizeof(common_helper));
    if(helper == NULL){
        perror("Create_common_memory error\n");
        exit(-1);
    }

    // pozbywamy sie smieci
    for (int i = 0; i < TABLE_N; ++i) {
        helper->table[i] = -1;
    }

    for (int i = 0; i < FURNANCE_N; ++i) {
        helper->furnance[i] = -1;
    }

    helper -> to_put_table = 0;
    helper -> to_put_furnance = 0;
    helper -> to_take_table = 0;
    helper -> to_take_furnance = 0;

    //tworzymy semafory
    f_entrance = create_semafor("f_entrance.txt" , 1);
    f_free = create_semafor("f_free.txt", 5);
    f_taken = create_semafor("f_taken.txt",  0);

    t_entrance = create_semafor("t_entrance.txt", 1);
    t_free = create_semafor("t_free.txt", 5);
    t_taken = create_semafor("t_taken.txt", 0);

    printf("Workers getting ready... \n");

    // creating workers
    for (int i = 0; i < p_makers; ++i) {
        pid_t pid;
        if((pid=fork()) == 0) {
            execlp("./pizza_maker", "./pizza_maker", NULL);
            printf("this printf is not expected... \n");
        }
        p_maker_pids[i] = pid;
    }

    for (int i = 0; i < drivers; ++i) {
        pid_t pid;
        if((pid=fork()) == 0) {
            execlp("./driver", "./driver", NULL);
            printf("this printf is not expected... \n");
        }
        drivers_pids[i] = pid;
    }

    // po przerwaninu dzialania restauracji zbaijamy pracownikow :)
    signal(SIGINT, close_pizzeria);
}

int main(int argc, char **argv) {
    if(argc !=3){
        perror("Invalid number of arguments!\n");
        exit(-1);
    }
    p_makers = atoi(argv[1]);
    drivers = atoi(argv[2]);

    p_maker_pids = (int*)calloc(p_makers, sizeof(int));
    drivers_pids = (int*)calloc(drivers, sizeof(int));

    open_pizzeria();
    while(1){};
    close_pizzeria();
}
