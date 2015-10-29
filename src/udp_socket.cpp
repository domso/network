#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h>
#include<arpa/inet.h>
#include <unistd.h>
#include<sys/socket.h>
#include "network/udp_socket.h"


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_socket::udp_socket() {
    dataPTR_ = new udp_socket_data();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_socket::udp_socket ( const network::udp_socket& that ) {
    dataPTR_ = that.dataPTR_;
    sem_post ( & ( dataPTR_->semaphore ) );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_socket::~udp_socket() {
    if ( dataPTR_ != nullptr ) {
        sem_wait ( & ( dataPTR_->semaphore ) );
        int tmp = 0;
        sem_getvalue ( & ( dataPTR_->semaphore ), &tmp );
        if ( tmp <= 0 ) {
            delete dataPTR_;
        }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_socket::operator= ( const network::udp_socket that ) {
    dataPTR_ = that.dataPTR_;
    sem_post ( & ( dataPTR_->semaphore ) );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool network::udp_socket::init ( const uint16_t PORT, const int family) {
    return dataPTR_->init(PORT, family);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_socket::shutRD() const {
    shutdown ( dataPTR_->skt, SHUT_RD );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_socket::shutWR() const {
    shutdown ( dataPTR_->skt, SHUT_WR );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_socket::shutRDWR() const {
    shutdown ( dataPTR_->skt, SHUT_RDWR );
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
uint16_t network::udp_socket::getPort() const {
    return dataPTR_->port;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool network::udp_socket::isValid() const {
    return dataPTR_->valid;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int network::udp_socket::setTimeout (const int sec ) const {
    struct timeval tv;
    tv.tv_sec = sec;
    setsockopt(dataPTR_->skt, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&tv, sizeof(struct timeval));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ssize_t network::udp_socket::send (const network::ip_addr& address, const std::vector< char >& buffer, const int msglen, const  int flags ) const {
    switch (dataPTR_->sin_family) {
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case AF_INET:
            if ( msglen > 0 ) {
                return sendto(dataPTR_->skt, buffer.data(), msglen, flags, ( sockaddr* ) &address.getSockaddr_in(), sizeof ( address.getSockaddr_in() ) );
            }
            return sendto(dataPTR_->skt, buffer.data(), buffer.size(), flags, ( sockaddr* ) &address.getSockaddr_in(), sizeof ( address.getSockaddr_in() ) );
            break;
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case AF_INET6:
            if ( msglen > 0 ) {
                return sendto(dataPTR_->skt, buffer.data(), msglen, flags, ( sockaddr* ) &address.getSockaddr_in6(), sizeof ( address.getSockaddr_in6() ) );
            }
            return sendto(dataPTR_->skt, buffer.data(), buffer.size(), flags, ( sockaddr* ) &address.getSockaddr_in6(), sizeof ( address.getSockaddr_in6() ) );
            break;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ssize_t network::udp_socket::send(const network::ip_pkg& pkg, const int flags) const {
    switch (dataPTR_->sin_family) {
        case AF_INET:
            return sendto(dataPTR_->skt, pkg.getData().data(), pkg.getData().size(), flags, ( sockaddr* ) &pkg.getAddr().getSockaddr_in(), sizeof ( pkg.getAddr().getSockaddr_in() ) );
            break;
        case AF_INET6:
            return sendto(dataPTR_->skt, pkg.getData().data(), pkg.getData().size(), flags, ( sockaddr* ) &pkg.getAddr().getSockaddr_in6(), sizeof ( pkg.getAddr().getSockaddr_in6() ) );
            break;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ssize_t network::udp_socket::recv (const network::ip_addr& address, std::vector< char >& buffer, const int flags ) const {
    socklen_t address_len;
    switch (dataPTR_->sin_family) {
        case AF_INET:
            address_len = sizeof ( address.getSockaddr_in() );
            return recvfrom(dataPTR_->skt, buffer.data(), buffer.size(), flags, ( sockaddr* ) &address.getSockaddr_in(), &address_len );
            break;
        case AF_INET6:
            address_len = sizeof ( address.getSockaddr_in6() );
            return recvfrom(dataPTR_->skt, buffer.data(), buffer.size(), flags, ( sockaddr* ) &address.getSockaddr_in6(), &address_len );
            break;
    }

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_socket::udp_socket_data::udp_socket_data () {
    sem_init (&semaphore, 0, 1 );
    valid = false;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_socket::udp_socket_data::~udp_socket_data() {
    sem_destroy (&semaphore );
    if (valid) {
        close (skt);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool network::udp_socket::udp_socket_data::init ( const uint16_t PORT, const int family) {
    sin_family = family;
    port = PORT;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    switch (sin_family) {
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case AF_INET:
            static_cast<ipv4_addr*>(&local_address)->init("", PORT);
            if ((skt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) != -1) {
                if (bind(skt, (sockaddr*)&local_address.getSockaddr_in(), sizeof(local_address.getSockaddr_in())) != -1) {
                    valid = true;
                }
            }
            static_cast<ipv4_addr*>(&local_address)->setValid(valid);
            break;
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        case AF_INET6:
            static_cast<ipv6_addr*>(&local_address)->init("", PORT);
            if ((skt = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) != -1) {
                if (bind(skt, (sockaddr*)&local_address.getSockaddr_in6(), sizeof(local_address.getSockaddr_in6())) != -1) {
                    valid = true;
                }
            }
            static_cast<ipv6_addr*>(&local_address)->setValid(valid);
            break;
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    return valid;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


