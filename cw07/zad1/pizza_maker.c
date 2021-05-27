//
// Created by kkoz34 on 12.05.2021.
//
#include "semafors.h"

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
    disconnect_memory(helper);
}



void start_work() {
    printf("Pizza maker works %d\n", getpid());

    // przydzielamy pamiec
     helper = create_common_memory("helper.txt", FURNANCE_N);

    f_entrance = get_semafor("f_entrance.txt");
    f_taken = get_semafor("f_taken.txt");
    f_free = get_semafor("f_free.txt");

    t_entrance = get_semafor("t_entrance.txt");
    t_taken = get_semafor("t_taken.txt");
    t_free = get_semafor("t_free.txt");

    // handler
    signal(SIGINT, end_work);

}

int work() {

    // wybor pizzy + czekanie
    int pizza = rand() % PIZZA_N;

    // czekanie na wolne miejsce
    wait_semafor(f_free);

    // czekanie az inny kucharz skonczy prace z piecem
    wait_semafor(f_entrance);

    //wkladamy helper->to_put_furnance
    int my_pizza_place = helper -> to_put_furnance;
    helper -> furnance[helper->to_put_furnance] = pizza;
    print_put_in(pizza, helper->to_put_furnance);
    helper -> to_put_furnance = (helper->to_put_furnance + 1) % FURNANCE_N;


    // zamykamy piec
    post_semafor(f_taken);
    post_semafor(f_entrance);

    // czekamy
    int to_wait = get_wait_seconds(1, 2);
    sleep(to_wait);


    // czas odebrac pizze
    wait_semafor(f_taken);
    wait_semafor(f_entrance);

    //trzymamy pizze w rekach
    print_take_out(pizza, my_pizza_place);
    helper->furnance[my_pizza_place] = -1;

    post_semafor(f_entrance);
    post_semafor(f_free);

    // poloz pizze na stolik
    wait_semafor(t_free);
    wait_semafor(t_entrance);

    //poloz na stoliku w miejscu do polozenia
    helper-> table[helper->to_put_table] = pizza;
    helper -> to_put_table = (helper->to_put_table + 1) % TABLE_N;


    post_semafor(t_entrance);
    post_semafor(t_taken);
}

int main(int argc, char **argv) {
    start_work();
    while (1) {
        work();
    }
    end_work();
}
