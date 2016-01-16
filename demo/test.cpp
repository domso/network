
#include "network/rw_mutex.h"
#include "network/udp_receiver.h"
#include "network/smart_ptr.h"
#include <mutex>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

volatile int tmp = 0;

#define DELAY_READ 1000
#define DELAY_WRITE 10
#define MAX_COUNT 100

void read_func(rw_mutex::read& read, pthread_barrier_t* barrier) {
    pthread_barrier_wait(barrier);
    while (true) {
        read.lock();
        std::this_thread::sleep_for(std::chrono::nanoseconds(DELAY_READ));
        if (tmp == MAX_COUNT) {
            read.unlock();
            pthread_barrier_wait(barrier);
            return;
        }
        read.unlock();
    }
}

void write_func(rw_mutex::write& write, pthread_barrier_t* barrier) {
    pthread_barrier_wait(barrier);
    bool run = true;
    while (run) {
        write.lock();
        std::this_thread::sleep_for(std::chrono::nanoseconds(DELAY_WRITE));
        if (tmp < MAX_COUNT) {
            tmp++;
        } else {
            run = false;
        }
        write.unlock();
    }
    pthread_barrier_wait(barrier);
}

void read_func2(std::mutex& read, pthread_barrier_t* barrier) {
    pthread_barrier_wait(barrier);
    while (true) {
        read.lock();
        std::this_thread::sleep_for(std::chrono::nanoseconds(DELAY_READ));
        if (tmp == MAX_COUNT) {
            read.unlock();
            pthread_barrier_wait(barrier);
            return;
        }
        read.unlock();
    }
}

void write_func2(std::mutex& write, pthread_barrier_t* barrier) {
    pthread_barrier_wait(barrier);
    bool run = true;
    while (run) {
        write.lock();
        std::this_thread::sleep_for(std::chrono::nanoseconds(DELAY_WRITE));
        if (tmp < MAX_COUNT) {
            tmp++;
        } else {
            run = false;
        }
        write.unlock();
    }
    pthread_barrier_wait(barrier);
}

int foo(network::ipv4_pkg t) {
    std::cout << "foo" << std::endl;
    return 0;
}

class A : public smart_ptr {

public:
    A() {
        setNewData(new data());
    }
private:

    struct data : smart_ptr_data {
        int a;


    };

public:
    data* operator->() {
        return (data*)getData();
    }

};


int main() {

    A a;
    a->a = 10;
    std::cout << a->a << std::endl;
    
    return 0;
    rw_mutex rwmutex;
    rw_mutex::read read(rwmutex);
    rw_mutex::write write(rwmutex);

    pthread_barrier_t barrier;
    int num = 4;
    pthread_barrier_init(&barrier, 0, num + 1);


    std::mutex mutex;

    int summe1 = 0;
    int summe2 = 0;
    int num_it = 100;

    for (int it = 0; it < num_it; it++) {
        std::chrono::high_resolution_clock::time_point t1;
        std::chrono::high_resolution_clock::time_point t2;


        std::vector<std::thread> threads(num);



        tmp = 0;
        threads[0] = std::thread(write_func, std::ref(write),  &barrier);

        for (int i = 1; i < num; i++) {
            threads[i] = std::thread(read_func, std::ref(read),  &barrier);
        }

        pthread_barrier_wait(&barrier);
        t1 = std::chrono::high_resolution_clock::now();
        pthread_barrier_wait(&barrier);
        t2 = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < num; i ++) {
            if (threads[i].joinable()) {
                threads[i].join();
            }
        }

        auto duration =  std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

//         std::cout << duration << std::endl;
        tmp = 0;
        threads[0] = std::thread(write_func2, std::ref(mutex),  &barrier);

        for (int i = 1; i < num; i++) {
            threads[i] = std::thread(read_func2, std::ref(mutex),  &barrier);
        }

        pthread_barrier_wait(&barrier);
        t1 = std::chrono::high_resolution_clock::now();
        pthread_barrier_wait(&barrier);
        t2 = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < num; i ++) {
            if (threads[i].joinable()) {
                threads[i].join();
            }
        }

        auto duration2 =  std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();


//         std::cout << duration2 << std::endl;


        summe1 += duration;
        summe2 += duration2;
    }
    double result = (double)(summe2 - summe1) / (double) num_it;
    double result2 = ((double) summe2 / (double) summe1) * 100;
    std::cout << "+: " << result2 << "%" << std::endl;

}
