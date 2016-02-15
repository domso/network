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
    if (dataPTR_ != nullptr) {
        sem_wait(& (dataPTR_->semaphore));
        int tmp = 0;
        sem_getvalue(& (dataPTR_->semaphore), &tmp);
        if (tmp <= 0) {
            delete dataPTR_;
        }
    }
    dataPTR_ = that.dataPTR_;
    sem_post(& (dataPTR_->semaphore));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::init(const network::udp_socket socket, void (*const work_callbackFunction)(ip_pkg&, const udp_socket&, const void* addPtr), const udp_receiver::udp_receiver_init_param* const parameters) const {
    dataPTR_->init(socket, work_callbackFunction, parameters);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::stop() {
    dataPTR_->threadState.stop();
    if (dataPTR_->cont_thread.joinable()) {
        dataPTR_->cont_thread.join();
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_receiver::udp_receiver_data::udp_receiver_data() {
    sem_init(&semaphore, 0, 1);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_receiver::udp_receiver_data::~udp_receiver_data() {
    sem_destroy(&semaphore);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_receiver::udp_receiver_init_param::udp_receiver_init_param() {
    bufferSize = NETWORK_UDP_RECEIVER_INIT_PARAM_BUFFERSIZE;
    sec2wait = NETWORK_UDP_RECEIVER_INIT_PARAM_SEC2WAIT;
    minThread = NETWORK_UDP_RECEIVER_INIT_PARAM_MINTHREAD;
    maxThread = NETWORK_UDP_RECEIVER_INIT_PARAM_MAXTHREAD;
    numThread = NETWORK_UDP_RECEIVER_INIT_PARAM_NUMTHREAD;
    addPtr = nullptr;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::udp_receiver_data::init(const network::udp_socket skt, void (*const work_cbFunction)(ip_pkg&, const udp_socket&, const void* addPtr), const udp_receiver::udp_receiver_init_param* const parameters) {
    socket = skt;
    work_callbackFunction = work_cbFunction;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    if (parameters != nullptr) {
        // set config
        param = *parameters;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    buffer.resize(param.bufferSize, '\0');
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    threadState.start();
    cont_thread = std::thread(contThread, this, param.sec2wait);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::udp_receiver_data::recvThread(network::udp_receiver::udp_receiver_data* receiver, int threadID) {
    int recvBytes = 0;
    int priority;
    udp_receiver recv(receiver);

    ip_pkg pkg;
    if (receiver->socket.getFamily() == AF_INET) {
        ipv4_pkg pkg_v4(receiver->param.bufferSize, nullptr);
        pkg = pkg_v4;
    } else {
        ipv6_pkg pkg_v6(receiver->param.bufferSize, nullptr);
        pkg = pkg_v6;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    while (receiver->threadState.isRunning()) {
        if ((recvBytes = receiver->socket.recv(pkg.getAddr(), pkg.getData())) > 0) {
            pkg.setLength(recvBytes);
            pkg.getAddr().update();
            receiver->work_callbackFunction(pkg, receiver->socket, receiver->param.addPtr);
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::udp_receiver_data::contThread(network::udp_receiver::udp_receiver_data* receiver , int pollIntervall) {
    udp_receiver recv(receiver);
    receiver->socket.setTimeout(pollIntervall);
    //std::vector<std::thread> threads(receiver->param.minThread);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    for (int tID = 0; tID < receiver->param.numThread; tID++) {
        std::thread thread(recvThread, receiver, );
        thread.detach();
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /*for (int i = 0; i < threads.size() ; i++) {
        if (threads[i].joinable()) {
            threads[i].join();
        }
    }*/
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

