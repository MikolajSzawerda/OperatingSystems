#include "shared.h"


int running;


void handler(int num){
    // write(STDOUT_FILENO, "Producer killed\n", 16);
    running=0;
}


void produce_item(){
        usleep(200);
}


int main(int argc, char* argv[]){
    signal(SIGUSR1, handler);
    running = 1;
    pid_t pid = getpid();
    int id = atoi(argv[1]);
    int productID = atoi(argv[2]);
    int* mem = get_mem(id);
    struct semaphores sems = get_semaphores(productID, 0);
    while(running) {
        produce_item();
        sem_wait(sems.product_producer_empty);
        sem_wait(sems.product_mutex);
        printf("PID: %d produce item: %d, curr val: %d\n", pid, productID, ++mem[productID]);
        sem_post(sems.product_mutex);
        sem_post(sems.product_producer_full);
    }
    close_semaphores(sems);
    return 0;
}