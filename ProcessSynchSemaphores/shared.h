#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define BUFFER_SIZE 10
#define DOUGH 0
#define MEAT 1
#define CHEESE 2
#define CABBAGE 3
#define DUMPLINGS_COUNT productID + CABBAGE


#define SEM_COUNT 12

#define SEM_MUTEX_DOUGH "/mutexDough"
#define SEM_MUTEX_MEAT "/mutexMeat"
#define SEM_MUTEX_CHEESE "/mutexCheese"
#define SEM_MUTEX_CABBAGE "/mutexCabbage"


#define SEM_EMPTY_DOUGH "/emptyDough"
#define SEM_EMPTY_MEAT "/emptyMeat"
#define SEM_EMPTY_CHEESE "/emptyCheese"
#define SEM_EMPTY_CABBAGE "/emptyCabbage"


#define SEM_FULL_DOUGH "/fullDough"
#define SEM_FULL_MEAT "/fullMeat"
#define SEM_FULL_CHEESE "/fullCheese"
#define SEM_FULL_CABBAGE "/fullCabbage"



#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE);} while (0)

const char* all_semaphores_names[SEM_COUNT] = {
    SEM_EMPTY_DOUGH, SEM_EMPTY_MEAT, SEM_EMPTY_CHEESE, SEM_EMPTY_CABBAGE,
    SEM_FULL_DOUGH, SEM_FULL_MEAT, SEM_FULL_CHEESE, SEM_FULL_CABBAGE,
    SEM_MUTEX_DOUGH, SEM_MUTEX_MEAT, SEM_MUTEX_CHEESE, SEM_MUTEX_CABBAGE
};



const int sems_init_values[SEM_COUNT] = {
    BUFFER_SIZE, BUFFER_SIZE, BUFFER_SIZE, BUFFER_SIZE,
    0, 0, 0, 0,
    1, 1, 1, 1
};

int init_mem(){
    int id = shmget(IPC_PRIVATE, 20*sizeof(int), IPC_CREAT | IPC_EXCL | 0600);
    if (id == -1) errExit("shmget");
    return id;
}

int* get_mem(int id){
    int* shared = shmat(id, NULL, 0);
    if (shared == (void *) -1) errExit("shmat");
    return shared;
}

void destroy_mem(int id){
    if (shmctl(id, IPC_RMID, NULL) == -1) errExit("shmctl");
}

struct semaphores{
    sem_t* dough_producer_empty;
    sem_t* dough_producer_full;
    sem_t* dough_mutex;
    sem_t* product_producer_empty;
    sem_t* product_producer_full;
    sem_t* product_mutex;
};

#define EMPTY productID
#define FULL EMPTY + 4
#define MUTEX FULL + 4
struct semaphores get_semaphores(int productID, int init_all){
    struct semaphores sems;

    sems.product_producer_empty = sem_open(all_semaphores_names[EMPTY], O_CREAT);
    if(sems.product_producer_empty == SEM_FAILED) errExit("init sem");

    sems.product_producer_full = sem_open(all_semaphores_names[FULL], O_CREAT);
    if(sems.product_producer_full == SEM_FAILED) errExit("init sem");

    sems.product_mutex = sem_open(all_semaphores_names[MUTEX], O_CREAT);
    if(sems.product_mutex == SEM_FAILED) errExit("init sem");

    if(init_all){
        sems.dough_producer_empty = sem_open(SEM_EMPTY_DOUGH, O_CREAT);
        if(sems.dough_producer_empty == SEM_FAILED) errExit("init sem");
        sems.dough_producer_full = sem_open(SEM_FULL_DOUGH, O_CREAT);
        if(sems.dough_producer_full == SEM_FAILED) errExit("init sem");
        sems.dough_mutex = sem_open(SEM_MUTEX_DOUGH, O_CREAT);
        if(sems.dough_mutex == SEM_FAILED) errExit("init sem");
    }
    return sems;
}

void close_semaphores(struct semaphores sems){
    sem_close(sems.dough_producer_empty);
    sem_close(sems.dough_producer_full);
    sem_close(sems.dough_mutex);
    sem_close(sems.product_producer_empty);
    sem_close(sems.product_producer_full);
    sem_close(sems.product_mutex);
}

