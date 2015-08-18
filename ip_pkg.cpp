#include "ip_pkg.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::ip_pkg::ip_pkg ( std::vector<char>& buffer,int len, ip_addr& addr ) {
    dataPTR_ = new ip_pkg_data ( buffer, len, addr );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::ip_pkg::ip_pkg ( const network::ip_pkg& that ) {
    dataPTR_ = that.dataPTR_;
    sem_post ( & ( dataPTR_->semaphore ) );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::ip_pkg::~ip_pkg() {
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
std::string network::ip_pkg::toString() {
    return "[Package][" + std::to_string ( dataPTR_->data.size() ) + " Bytes][" + dataPTR_->addr.getIP() + ":" + std::to_string ( dataPTR_->addr.getPort() ) + "][Age: " + std::to_string(getAge()) + "]";
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::vector<char>& network::ip_pkg::getData() {
    return dataPTR_->data;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int network::ip_pkg::getAge() {
	std::chrono::high_resolution_clock::time_point t = std::chrono::high_resolution_clock::now();
	int tmp = std::chrono::duration_cast<std::chrono::microseconds>(t - dataPTR_->time).count();
	return tmp;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::ip_pkg::ip_pkg_data::ip_pkg_data ( std::vector< char >& buffer, int len, ip_addr& addr ) {
    sem_init ( &semaphore, 0, 1 );
    data.resize ( len,'\0' );
    for ( int i = 0; i < len ; i++ ) {
        data[i] = buffer[i];
    }
    time = std::chrono::high_resolution_clock::now();
    this->addr = addr;
    this->addr.update();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::ip_pkg::ip_pkg_data::~ip_pkg_data() {
    sem_destroy ( &semaphore );
}

