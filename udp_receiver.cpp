#include "udp_receiver.h"
#include "assert.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_receiver::udp_receiver () {
    dataPTR_ = new udp_receiver_data();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_receiver::udp_receiver ( const network::udp_receiver& that ) {
    dataPTR_ = that.dataPTR_;
    sem_post ( & ( dataPTR_->semaphore ) );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_receiver::udp_receiver ( network::udp_receiver::udp_receiver_data* dataPTR ) {
	assert( dataPTR != nullptr);
	dataPTR_ = dataPTR;
	sem_post ( & ( dataPTR_->semaphore ) );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_receiver::~udp_receiver() {
    if ( dataPTR_ != nullptr ) {
        sem_wait ( & ( dataPTR_->semaphore ) );
        int tmp = 0;
        sem_getvalue ( & ( dataPTR_->semaphore ),&tmp );
        if ( tmp <= 0 ) {
            delete dataPTR_;
        }
    }
    
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::init ( network::udp_socket socket,  int bufferSize,int ( *recv_callbackFunction ) ( ip_addr&,std::vector<char>&,int,udp_socket ), void ( *work_callbackFunction ) ( ip_pkg, udp_socket ), int minThread, int maxThread ) {
    dataPTR_->init ( socket, bufferSize, recv_callbackFunction, work_callbackFunction, minThread, maxThread );

}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::stop() {
    dataPTR_->socket.shutRDWR();
    if ( dataPTR_->recv_thread.joinable() ) {
        dataPTR_->recv_thread.join();
    }
    dataPTR_->queue.setMSG(-1);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_receiver::udp_receiver_data::udp_receiver_data() {
    sem_init ( &semaphore,0,1 );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_receiver::udp_receiver_data::~udp_receiver_data() {
	/*
    socket.shutRDWR();
    if ( recv_thread.joinable() ) {
        recv_thread.join();
    }
	*/
    sem_destroy ( &semaphore );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::udp_receiver_data::init ( network::udp_socket skt, int bufferSize,int ( *recv_cbFunction ) ( ip_addr&,std::vector<char>&,int,udp_socket ), void ( *work_cbFunction ) ( ip_pkg, udp_socket ), int minThread, int maxThread ) {
    socket = skt;
    buffer.resize ( bufferSize,'\0' );
    recv_callbackFunction = recv_cbFunction;
	work_callbackFunction = work_cbFunction;
    recv_thread = std::thread ( recvThread,this );
	assert(maxThread >= minThread);
	sem_init(&thread_sem,0,maxThread - minThread);
	
	sec2wait = 10;
	sec2close = 100;
	microsec2start = 1000;
	
	if(work_callbackFunction != nullptr){
		for (int i = 0 ; i < minThread ; i++){
			std::thread t(workThread,this,true);
			t.detach();
		}
	}
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::udp_receiver_data::recvThread ( network::udp_receiver::udp_receiver_data* receiver ) {
    int recvBytes = 0;

    while ( ( recvBytes = receiver->socket.recv ( receiver->recv_addr,receiver->buffer ) ) > 0 ) {
        if ( receiver->recv_callbackFunction == nullptr || receiver->recv_callbackFunction ( receiver->recv_addr,receiver->buffer,recvBytes,receiver->socket ) == 0 ) {
            ip_pkg pkg ( receiver->buffer,recvBytes,receiver->recv_addr );
            receiver->queue.push ( pkg );
        }
    }

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_receiver::udp_receiver_data::workThread ( network::udp_receiver::udp_receiver_data* receiver, bool isMainThread) {
	udp_receiver recv(receiver);
	int timeWaited = 0;
    while ( receiver->queue.getMSG() != -1 ) {
		if(receiver->queue.waitForData ( receiver->sec2wait )){
			timeWaited = 0;
			ip_pkg pkg = receiver->queue.getData();
			receiver->work_callbackFunction ( pkg,receiver->socket );
			
			if(pkg.getAge() > receiver->microsec2start){
				if(sem_trywait(&(receiver->thread_sem))==0){
					std::thread t(workThread,receiver,false);
					t.detach();
				}
			}
			
		}else{
			if(!isMainThread){
				timeWaited += receiver->sec2wait;
				if(timeWaited > receiver->sec2close){
					return;
				}
			}
		}		
    }
}

