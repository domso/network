#include "network/udp_receiver.h"
#include "assert.h"
#include "signal.h"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_receiver::udp_receiver() : dataPTR_(nullptr) {
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

    sem_wait(& (dataPTR_->semaphore));
    int tmp = 0;
    sem_getvalue(& (dataPTR_->semaphore), &tmp);
    if (tmp <= 0) {
        delete dataPTR_;
    }

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::operator= (const network::udp_receiver that) {
    dataPTR_ = that.dataPTR_;
    sem_post(& (dataPTR_->semaphore));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::init(const network::udp_socket socket, int (*const recv_callbackFunction)(const ip_addr&, std::vector<char>&, const int, const udp_socket&), void (*const work_callbackFunction)(ip_pkg&, const udp_socket&, const void* addPtr), const udp_receiver::udp_receiver_init_param* const parameters) const {
    dataPTR_->init(socket, recv_callbackFunction, work_callbackFunction, parameters);
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
network::udp_receiver::udp_receiver_data::udp_receiver_data() : queue(5) {
    sem_init(&semaphore, 0, 1);
    sem_init(&numThread, 0, 0);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_receiver::udp_receiver_data::~udp_receiver_data() {
    sem_destroy(&numThread);
    sem_destroy(&semaphore);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_receiver::udp_receiver_init_param::udp_receiver_init_param() {
    bufferSize = NETWORK_UDP_RECEIVER_INIT_PARAM_BUFFERSIZE;
    sec2wait = NETWORK_UDP_RECEIVER_INIT_PARAM_SEC2WAIT;
    sec2close = NETWORK_UDP_RECEIVER_INIT_PARAM_SEC2CLOSE;
    microsec2start = NETWORK_UDP_RECEIVER_INIT_PARAM_MICROSEC2START;
    minThread = NETWORK_UDP_RECEIVER_INIT_PARAM_MINTHREAD;
    maxThread = NETWORK_UDP_RECEIVER_INIT_PARAM_MAXTHREAD;
    numPriority = NETWORK_UDP_RECEIVER_INIT_PARAM_NUMPRIORITY;
    addPtr = nullptr;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::udp_receiver_data::init(const network::udp_socket skt, int (*const recv_cbFunction)(const ip_addr&, std::vector<char>&, const int, const udp_socket&), void (*const work_cbFunction)(ip_pkg&, const udp_socket&, const void* addPtr), const udp_receiver::udp_receiver_init_param* const parameters) {
    socket = skt;
    recv_callbackFunction = recv_cbFunction;
    work_callbackFunction = work_cbFunction;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (parameters == nullptr) {
        // default config
        bufferSize = NETWORK_UDP_RECEIVER_INIT_PARAM_BUFFERSIZE;
        sec2wait = NETWORK_UDP_RECEIVER_INIT_PARAM_SEC2WAIT;
        sec2close = NETWORK_UDP_RECEIVER_INIT_PARAM_SEC2CLOSE;
        microsec2start = NETWORK_UDP_RECEIVER_INIT_PARAM_MICROSEC2START;
        minThread = NETWORK_UDP_RECEIVER_INIT_PARAM_MINTHREAD;
        maxThread = NETWORK_UDP_RECEIVER_INIT_PARAM_MAXTHREAD;
        numPriority = NETWORK_UDP_RECEIVER_INIT_PARAM_NUMPRIORITY;
        addPtr = nullptr;
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    } else {
        // set config
        bufferSize = parameters->bufferSize;
        sec2wait = parameters->sec2wait;
        sec2close = parameters->sec2close;
        microsec2start = parameters->microsec2start;
        minThread = parameters->minThread;
        maxThread = parameters->maxThread;
        numPriority = parameters->numPriority;
        addPtr = parameters->addPtr;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    buffer.resize(bufferSize, '\0');
    sem_init(&dynamicThreadNum, 0, maxThread - minThread);
    queue.setPriority(numPriority);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    threadState.start();
    cont_thread = std::thread(contThread, this, sec2wait);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::udp_receiver_data::recvThread(network::udp_receiver::udp_receiver_data* receiver , int pollIntervall) {
    int recvBytes = 0;
    int priority;
    udp_receiver recv(receiver);
    receiver->socket.setTimeout(pollIntervall);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    while (receiver->threadState.isRunning()) {
        if ((recvBytes = receiver->socket.recv(receiver->recv_addr, receiver->buffer)) > 0) {
            priority = 0;
            if (receiver->recv_callbackFunction == nullptr || (priority = receiver->recv_callbackFunction(receiver->recv_addr, receiver->buffer, recvBytes, receiver->socket)) >= 0 ) {
                ip_pkg pkg(receiver->buffer, recvBytes, receiver->recv_addr);
                receiver->queue.push(pkg, priority);
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
            receiver->work_callbackFunction(pkg, receiver->socket, receiver->addPtr);
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
    std::vector<std::thread> threads(receiver->minThread + 1);
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
            threads.push_back(std::thread(workThread, receiver, false));
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    for (int i = 0; i < threads.size() ; i++) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

