#include <assert.h>
#include "shared.h"
#define N_AGENT 7

sem_t* all_sems[SEM_COUNT];


struct agent{
    int count;
    char proc_name[32];
    char name[32];
    int product_type;
};

int start_agents(int mem_id, struct agent agents[N_AGENT], int* pids){
    int pid;
    for(int i=0, k=0;i<N_AGENT;i++){
        struct agent a = agents[i];
        for(int j=0;j<a.count;j++,k++){
            pid = fork();
            if(!pid){
                char buff[32];
                char productBuff[32];
                sprintf(buff, "%d", mem_id);
                sprintf(productBuff, "%d", a.product_type);
                char* args[]={a.name, buff, productBuff, NULL};
                execv(a.proc_name, args);
                return pid;
            } else pids[k] = pid;
        }

    }
    return pid;
}

void init_semaphores(){
    for(int i=0;i<SEM_COUNT;i++){
        sem_unlink(all_semaphores_names[i]);
        all_sems[i] = sem_open(all_semaphores_names[i], O_CREAT | O_EXCL, 0600, sems_init_values[i]);
        if(all_sems[i] == SEM_FAILED) errExit("init_semaphores");
    }
}

void destroy_semaphores(){
    for(int i=0;i<SEM_COUNT;i++){
        sem_close(all_sems[i]);
        sem_unlink(all_semaphores_names[i]);
    }
}

void run_factory(){
    sleep(4);
}

void save_results(int* mem){
    struct semaphores sems = get_semaphores(0, 0);
    FILE* fptr = fopen("output.txt", "w");
    if(fptr==NULL) errExit("File open");
    int productID = MEAT;
    sem_wait(sems.product_mutex);
    fprintf(fptr, "Meat: %d\n", mem[DUMPLINGS_COUNT]);
    productID = CHEESE;
    fprintf(fptr, "Cheese: %d\n", mem[DUMPLINGS_COUNT]);
    productID = CABBAGE;
    fprintf(fptr, "Cabbage: %d\n", mem[DUMPLINGS_COUNT]);
    sem_post(sems.product_mutex);
    fclose(fptr);
}


int main(int argc, char* argv[]) {
    int pid;
    printf("%d\n", atoi(argv[7]));
    struct agent agents[] = {
        {atoi(argv[1]), "./consumer", "consumer", MEAT},
        {atoi(argv[2]), "./consumer", "consumer", CHEESE},
        {atoi(argv[3]), "./consumer", "consumer", CABBAGE},
        {atoi(argv[4]), "./producer", "producer", DOUGH},
        {atoi(argv[5]), "./producer", "producer", MEAT},
        {atoi(argv[6]), "./producer", "producer", CHEESE},
        {atoi(argv[7]), "./producer", "producer", CABBAGE},
    };
    int n = sizeof(agents)/sizeof(struct agent);
    assert(n==N_AGENT);
    int agents_cont = 0;
    for(int i=0;i<N_AGENT;i++) agents_cont+=agents[i].count;
    int pids[agents_cont];
    int id = init_mem();
    int* mem = get_mem(id);

    init_semaphores();

    pid=start_agents(id, agents, pids);

    run_factory();

    for(int i=0;i<agents_cont;i++) kill(pids[i], SIGUSR1);
    wait(NULL);

    save_results(mem);

    if(pid){
        destroy_mem(id);
        destroy_semaphores();
    }
    return 0;
}
