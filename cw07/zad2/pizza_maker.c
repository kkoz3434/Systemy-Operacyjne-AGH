//
// Created by kkoz34 on 12.05.2021.
//
#include "semafores.h"

//memory
common_helper *helper;

// semafors
int f_entrance, t_entrance;
int f_free, f_taken, t_free, t_taken;

//pizza index
int pizza_to_take;

void print_put_table(int pizza, int place) {
    printf("Pizza Maker: %d puts pizza %d on table\n", getpid(), pizza);
    printf("    Place for pizza: %d \n", place);
    printf("    Time: %s\n", get_time());
}

void print_put_in(int pizza, int place) {
    printf("Pizza Maker: %d puts pizza %d in furnance\n", getpid(), pizza);
    printf("    Place for pizza: %d \n", place);
    printf("    Time: %s\n", get_time());
}

void print_take_out(int pizza, int place) {
    printf("Pizza Maker: %d takes pizza %d out furnance\n", getpid(), pizza);
    printf("    Place from furnance pizza: %d \n", place);
    printf("    Time: %s\n", get_time());
}

void end_work() {
    disconnect_memory(helper, sizeof(common_helper));

    // Po zakończeniu pracy z semaforem należy go zamknąć. Służy do tego funkcja sem_close
    close_semafor(f_entrance);
    close_semafor(f_free);
    close_semafor(f_taken);

    close_semafor(t_entrance);
    close_semafor(t_free);
    close_semafor(t_taken);
}



void start_work() {
    printf("Pizza maker works %d\n", getpid());

    // przydzielamy pamiec
    helper = create_common_memory("helper.txt", sizeof(common_helper));

    f_entrance = open_existing_semafore("f_entrance.txt");
    f_taken = open_existing_semafore("f_taken.txt");
    f_free = open_existing_semafore("f_free.txt");

    t_entrance = open_existing_semafore("t_entrance.txt");
    t_taken = open_existing_semafore("t_taken.txt");
    t_free = open_existing_semafore("t_free.txt");

    // handler
    signal(SIGINT, end_work);

}

int work() {

    // wybor pizzy + czekanie
    int pizza = rand() % PIZZA_N;

    // czekanie na wolne miejsce
    printf("1\n");
    sem_wait(f_free);
    printf("11\n");
    // czekanie az inny kucharz skonczy prace z piecem
    sem_wait(f_entrance);

    //wkladamy helper->to_put_furnance
    int my_pizza_place = helper -> to_put_furnance;
    helper -> furnance[helper->to_put_furnance] = pizza;
    print_put_in(pizza, helper->to_put_furnance);
    helper -> to_put_furnance = (helper->to_put_furnance + 1) % FURNANCE_N;


    // zamykamy piec
    sem_post(f_taken);
    sem_post(f_entrance);
    printf("111\n");

    // czekamy
    int to_wait = get_wait_seconds(1, 2);
    sleep(to_wait);

    printf("2\n");
    // czas odebrac pizze
    sem_wait(f_taken);
    sem_wait(f_entrance);


    //trzymamy pizze w rekach
    print_take_out(pizza, my_pizza_place);
    helper->furnance[my_pizza_place] = -1;

    sem_post(f_entrance);
    sem_post(f_free);
    printf("22\n");

    // poloz pizze na stolik
    sem_wait(t_free);
    sem_wait(t_entrance);

    //poloz na stoliku w miejscu do polozenia
    helper-> table[helper->to_put_table] = pizza;
    print_put_table(pizza, helper->to_put_table);
    helper -> to_put_table = (helper->to_put_table + 1) % TABLE_N;


    sem_post(t_entrance);
    sem_post(t_taken);
}

int main(int argc, char **argv) {
    start_work();
    while (1) {
        work();
    }
    end_work();
}