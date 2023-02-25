#include "shared.h"

int running;


void handler(int num){
    // write(STDOUT_FILENO, "Consumer killed\n", 16);
    running=0;
}


void consume_item(){
        usleep(100);
}


int main(int argc, char* argv[]){
    signal(SIGUSR1, handler);
    running=1;
    pid_t pid = getpid();
    int id = atoi(argv[1]);
    int productID = atoi(argv[2]);
    int* mem = get_mem(id);
    struct semaphores sems = get_semaphores(productID, 1);

    int dough = 0;
    int product_val = 0;
    int dumpling_count = 0;
    while(running){
        sem_wait(sems.product_producer_full);
        sem_wait(sems.dough_producer_full);

        sem_wait(sems.product_mutex);
        product_val = --mem[productID];
        sem_post(sems.product_mutex);

        sem_wait(sems.dough_mutex);
        dough = --mem[DOUGH];
        dumpling_count = ++mem[DUMPLINGS_COUNT];
        sem_post(sems.dough_mutex);

        printf("PID: %d consume dough: %d and product: %d val: %d dumplings: %d\n", pid, dough, productID, product_val, dumpling_count);

        sem_post(sems.dough_producer_empty);
        sem_post(sems.product_producer_empty);

        consume_item();
    }
    close_semaphores(sems);
    return 0;
}