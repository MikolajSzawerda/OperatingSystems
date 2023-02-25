#include <iostream>
#include <thread>
#include <sstream>
#include "vector"
#include "mutex"
#include "condition_variable"
#include "unistd.h"
#include "atomic"

#define BUFFER_SIZE 10
#define CONSUMER_RATE 100 //useconds
#define PRODUCER_RATE 400 //useconds
#define WORKING_TIME 5 //seconds

#define DOUGH 0
#define MEAT 1
#define CHEESE 2
#define CABBAGE 3
const std::string productNames[]{"DOUGH", "MEAT", "CHEESE", "CABBAGE"};

int dumplingsCount[3];
std::mutex countersMutex[3];

class Buffer {
private:
    std::mutex mutex;
    std::condition_variable full;
    std::condition_variable r_full;
    int count = 0;
    int reservation_count = 0;
public:
    int produce() {
        std::unique_lock lock(mutex);
        full.wait(lock, [this] { return count < BUFFER_SIZE; });
        count += 1;
        r_full.notify_one();
        return count;
    }

    void reserve() {
        std::unique_lock lock(mutex);
        r_full.wait(lock, [this] { return reservation_count < count; });
        reservation_count += 1;
    }

    int consume() {
        std::unique_lock lock(mutex);
        count -= 1;
        reservation_count -= 1;
        full.notify_one();
        return count;
    }
};

Buffer b[4];

void produce_item() {
    usleep(PRODUCER_RATE);
}

void consume_item(int i) {
    std::unique_lock lock(countersMutex[i - 1]);
    printf("%s dumpling count: %d\n", productNames[i].c_str(), ++dumplingsCount[i - 1]);
    lock.unlock();
    usleep(CONSUMER_RATE);
}

auto producer = [](std::atomic_bool &running, int i) {
    while (running) {
        produce_item();
        printf("Produce: %s val: %d\n", productNames[i].c_str(), b[i].produce());
    }
    printf("Producer %d killed\n", i);
};

auto consumer = [](std::atomic_bool &running, int i) {
    while (running) {
        b[i].reserve();
        b[DOUGH].reserve();
        int dough = b[DOUGH].consume();
        int product = b[i].consume();
        printf("Consume dumpling: %s dough: %d product: %d\n", productNames[i].c_str(), dough, product);
        consume_item(i);
    }
    printf("Consumer %d killed\n", i);
};

struct ThreadDescriptor {
    int count;
    int productType;
    char agentType;
};


int main(int argc, char *argv[]) {
    std::atomic_bool running = true;
    std::vector<ThreadDescriptor> thDesc = {
            {atoi(argv[1]), MEAT,    'c'},
            {atoi(argv[2]), CHEESE,  'c'},
            {atoi(argv[3]), CABBAGE, 'c'},
            {atoi(argv[4]), DOUGH,   'p'},
            {atoi(argv[5]), MEAT,    'p'},
            {atoi(argv[6]), CHEESE,  'p'},
            {atoi(argv[7]), CABBAGE, 'p'},
    };
    std::vector<std::thread> consumers;
    std::vector<std::thread> producers;
    for (auto &th: thDesc) {
        for (int i = 0; i < th.count; i++) {
            if (th.agentType == 'c') consumers.emplace_back(consumer, std::ref(running), th.productType);
            else producers.emplace_back(producer, std::ref(running), th.productType);
        }
    }
    sleep(WORKING_TIME);
    running = false;

    for (auto &c: consumers) c.join();
    for (auto &p: producers) p.join();
    for (int i = 0; i < 3; i++) {
        std::unique_lock lock(countersMutex[i]);
        printf("%s: %d\n", productNames[i].c_str(), dumplingsCount[i]);
    }
    return 0;
}
