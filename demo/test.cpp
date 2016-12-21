
#include "network/rw_mutex.h"
#include "network/udp_socket.h"
#include "network/smart_ptr.h"
#include <mutex>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>


int main() {
    
    network::udp_socket<network::ipv4_addr> socket;
    
    
}

/*
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

class A : public smart_ptr_ref {

public:

    A() {
//         std::cout << "constructor" << std::endl;
    }

    A(const A& that) {
//         std::cout << "copy-constructor" << std::endl;
    }
    ~A() {
//         std::cout << "destructor" << std::endl;
    }

    int a;
    int test() {
        return a;
    }


};

class B {

public:

    B() {
//         std::cout << "constructor" << std::endl;
    }

    B(const A& that) {
//         std::cout << "copy-constructor" << std::endl;
    }
    ~B() {
//         std::cout << "destructor" << std::endl;
    }

    int a;
    int test() {
        return a;
    }


};

void foo(smart_ptr<A> obj) {
    (*obj).test();
    obj->test();
}
void foo2(std::shared_ptr<B> obj) {
    (*obj).test();
    obj->test();
}
#define num_it 10000000
int main() {
    smart_ptr<A> a = new A();
    
    A& b = *a;
    
    

    std::chrono::high_resolution_clock::time_point t1;
    std::chrono::high_resolution_clock::time_point t2;

    int sum1 = 0;
    int sum2 = 0;

    double result1,  result2;
    
    for (int i = 0; i < num_it; i++) {

        t1 = std::chrono::high_resolution_clock::now();
        {
            std::shared_ptr<B> obj2 = std::make_shared<B>();
            foo2(obj2);
        }

        t2 = std::chrono::high_resolution_clock::now();


        auto duration =  std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
        sum1 += duration;


        t1 = std::chrono::high_resolution_clock::now();
        {

            smart_ptr<A> obj = new A();
            foo(obj);
        }

        t2 = std::chrono::high_resolution_clock::now();


        duration =  std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
        sum2 += duration;
    }
    result1 = (double)sum1/num_it;
    result2 = (double)sum2/num_it;
    std::cout << result1 << "/" << result2 << std::endl;
    return 0;
}

*/