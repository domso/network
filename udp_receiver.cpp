#include "udp_receiver.h"
#include "assert.h"
#include "signal.h"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_receiver::udp_receiver() {
    dataPTR_ = new udp_receiver_data();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_receiver::udp_receiver(const network::udp_receiver& that) {
    dataPTR_ = that.dataPTR_;
    sem_post(& (dataPTR_->semaphore));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_receiver::udp_receiver(network::udp_receiver::udp_receiver_data* dataPTR) {
    assert(dataPTR != nullptr);
    dataPTR_ = dataPTR;
    sem_post(& (dataPTR_->semaphore));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_receiver::~udp_receiver() {
    if (dataPTR_ != nullptr) {
        sem_wait(& (dataPTR_->semaphore));
        int tmp = 0;
        sem_getvalue(& (dataPTR_->semaphore), &tmp);
        if (tmp <= 0) {
            delete dataPTR_;
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::operator= (const network::udp_receiver that) {
    dataPTR_ = that.dataPTR_;
    sem_post(& (dataPTR_->semaphore));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::init(const network::udp_socket socket, const  int bufferSize, int (*const recv_callbackFunction)(const ip_addr&, std::vector<char>&, const int, const udp_socket), void (*const work_callbackFunction)(ip_pkg, const udp_socket), const int minThread, const int maxThread) const {
    dataPTR_->init(socket, bufferSize, recv_callbackFunction, work_callbackFunction, minThread, maxThread);

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::stop() {
    dataPTR_->threadState.stop();
    if (dataPTR_->cont_thread.joinable()) {
        dataPTR_->cont_thread.join();
    }
    sem_destroy(& (dataPTR_->dynamicThreadNum));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_receiver::udp_receiver_data::udp_receiver_data() {
    sem_init(&semaphore, 0, 1);
    sem_init(&numThread, 0, 0);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_receiver::udp_receiver_data::~udp_receiver_data() {
    sem_destroy(&numThread);
    sem_destroy(&semaphore);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::udp_receiver_data::init(const network::udp_socket skt, const int bufferSize, int (*const recv_cbFunction)(const ip_addr&, std::vector<char>&, const int, const udp_socket), void (*const work_cbFunction)(ip_pkg, const udp_socket), const int minThread, const int maxThread) {
    assert(maxThread >= minThread);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    buffer.resize(bufferSize, '\0');
    sem_init(&dynamicThreadNum, 0, maxThread - minThread);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    socket = skt;
    recv_callbackFunction = recv_cbFunction;
    work_callbackFunction = work_cbFunction;
    sec2wait = 10;
    sec2close = 100;
    microsec2start = 1000;
    this->minThread = minThread;
    this->maxThread = maxThread;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    threadState.start();
    cont_thread = std::thread(contThread, this, sec2wait);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::udp_receiver_data::recvThread(network::udp_receiver::udp_receiver_data* receiver , int pollIntervall) {
    int recvBytes = 0;
    receiver->socket.setTimeout(pollIntervall);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    while (receiver->threadState.isRunning()) {
        if ((recvBytes = receiver->socket.recv(receiver->recv_addr, receiver->buffer)) > 0) {
            if (receiver->recv_callbackFunction == nullptr || receiver->recv_callbackFunction(receiver->recv_addr, receiver->buffer, recvBytes, receiver->socket) == 0) {
                ip_pkg pkg(receiver->buffer, recvBytes, receiver->recv_addr);
                receiver->queue.push(pkg);
            }
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::udp_receiver_data::workThread(network::udp_receiver::udp_receiver_data* receiver, bool isMainThread) {
    udp_receiver recv(receiver);
    int timeWaited = 0;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    while (receiver->threadState.isRunning()) {
        if (receiver->queue.waitForData(receiver->sec2wait)) {
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            timeWaited = 0;
            ip_pkg pkg = receiver->queue.getData();
            receiver->work_callbackFunction(pkg, receiver->socket);
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            if (pkg.getAge() > receiver->microsec2start) {
                if (sem_trywait(& (receiver->dynamicThreadNum)) == 0) {
                    sem_post(& (receiver->numThread));
                }
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        } else {
            if (!isMainThread) {
                timeWaited += receiver->sec2wait;
                if (timeWaited > receiver->sec2close) {
                    return;
                }
            }
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::udp_receiver_data::contThread(network::udp_receiver::udp_receiver_data* receiver , int pollIntervall) {
    udp_receiver recv(receiver);
    std::vector<std::thread> threads(receiver->maxThread + 1);
    int tID = 0;
    threads[tID] = std::thread(recvThread, receiver, pollIntervall);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    for (tID = 1 ; tID < receiver->minThread + 1 ; tID++) {
        threads[tID] = std::thread(workThread, receiver, true);
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    timespec ts;
    assert(clock_gettime(CLOCK_REALTIME, &ts) != -1);
    ts.tv_sec += pollIntervall;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    while (receiver->threadState.isRunning()) {
        if (sem_timedwait(& (receiver->numThread), &ts) == 0) {
            if (tID < receiver->maxThread + 1) {
                threads[tID++] = std::thread(workThread, receiver, false);
            }
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    for (int i = 0; i < tID ; i++) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

