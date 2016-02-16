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
network::udp_socket::udp_socket(const network::udp_socket& that) {
    dataPTR_ = that.dataPTR_;
    sem_post(& (dataPTR_->semaphore));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_socket::~udp_socket() {
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
void network::udp_socket::operator= (const network::udp_socket that) {
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
bool network::udp_socket::init(const uint16_t PORT, const int family) {
    return dataPTR_->init(PORT, family);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_socket::shutRD() const {
    shutdown(dataPTR_->skt, SHUT_RD);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_socket::shutWR() const {
    shutdown(dataPTR_->skt, SHUT_WR);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void network::udp_socket::shutRDWR() const {
    shutdown(dataPTR_->skt, SHUT_RDWR);
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
int network::udp_socket::setTimeout(const float sec) const {
    struct timeval tv;
    tv.tv_sec = (int)sec;
    tv.tv_usec = (1000000.0*sec);
    tv.tv_usec = tv.tv_usec % 1000000;   
    setsockopt(dataPTR_->skt, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&tv, sizeof(struct timeval));
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int network::udp_socket::getFamily() const {
    return dataPTR_->sin_family;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ssize_t network::udp_socket::send(const network::ip_addr& address, const std::vector< char >& buffer, const int msglen, const  int flags) const {
    switch (dataPTR_->sin_family) {
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    case AF_INET: {
        const ipv4_addr& addr = static_cast<const ipv4_addr&>(address);
        if (msglen > 0) {
            return sendto(dataPTR_->skt, buffer.data(), msglen, flags, (sockaddr*) &addr.getSockaddr_in(), sizeof(addr.getSockaddr_in()));
        }
        return sendto(dataPTR_->skt, buffer.data(), buffer.size(), flags, (sockaddr*) &addr.getSockaddr_in(), sizeof(addr.getSockaddr_in()));
        break;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    case AF_INET6: {
        const ipv6_addr& addr6 = static_cast<const ipv6_addr&>(address);
        if (msglen > 0) {
            return sendto(dataPTR_->skt, buffer.data(), msglen, flags, (sockaddr*) &addr6.getSockaddr_in6(), sizeof(addr6.getSockaddr_in6()));
        }
        return sendto(dataPTR_->skt, buffer.data(), buffer.size(), flags, (sockaddr*) &addr6.getSockaddr_in6(), sizeof(addr6.getSockaddr_in6()));
        break;
    }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ssize_t network::udp_socket::send(const network::ip_addr& address, const std::string& buffer, const int msglen, const int flags) const {
    switch (dataPTR_->sin_family) {
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    case AF_INET: {
        const ipv4_addr& addr = static_cast<const ipv4_addr&>(address);
        if (msglen > 0) {
            return sendto(dataPTR_->skt, &buffer[0], msglen, flags, (sockaddr*) &addr.getSockaddr_in(), sizeof(addr.getSockaddr_in()));
        }
        return sendto(dataPTR_->skt, &buffer[0], buffer.length(), flags, (sockaddr*) &addr.getSockaddr_in(), sizeof(addr.getSockaddr_in()));
        break;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    case AF_INET6: {
        const ipv6_addr& addr = static_cast<const ipv6_addr&>(address);
        if (msglen > 0) {
            return sendto(dataPTR_->skt, &buffer[0], msglen, flags, (sockaddr*) &addr.getSockaddr_in6(), sizeof(addr.getSockaddr_in6()));
        }
        return sendto(dataPTR_->skt, &buffer[0], buffer.length(), flags, (sockaddr*) &addr.getSockaddr_in6(), sizeof(addr.getSockaddr_in6()));
        break;
    }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ssize_t network::udp_socket::send(const network::ip_pkg& pkg, const int flags) const {
    switch (dataPTR_->sin_family) {
    case AF_INET: {
        const ipv4_addr& addr = static_cast<const ipv4_addr&>(pkg.getAddr());
        return sendto(dataPTR_->skt, pkg.getData().data(), pkg.getData().size(), flags, (sockaddr*) &addr.getSockaddr_in(), sizeof(addr.getSockaddr_in()));
        break;
    }
    case AF_INET6: {
        const ipv6_addr& addr = static_cast<const ipv6_addr&>(pkg.getAddr());
        return sendto(dataPTR_->skt, pkg.getData().data(), pkg.getData().size(), flags, (sockaddr*) &addr.getSockaddr_in6(), sizeof(addr.getSockaddr_in6()));
        break;
    }
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ssize_t network::udp_socket::recv(const network::ip_addr& address, std::vector< char >& buffer, const int flags) const {
    socklen_t address_len;
    switch (dataPTR_->sin_family) {
    case AF_INET: {
        const ipv4_addr& addr = static_cast<const ipv4_addr&>(address);
        address_len = sizeof(addr.getSockaddr_in());
        return recvfrom(dataPTR_->skt, buffer.data(), buffer.size(), flags, (sockaddr*) &addr.getSockaddr_in(), &address_len);
        break;
    }
    case AF_INET6: {
        const ipv6_addr& addr = static_cast<const ipv6_addr&>(address);
        address_len = sizeof(addr.getSockaddr_in6());
        return recvfrom(dataPTR_->skt, buffer.data(), buffer.size(), flags, (sockaddr*) &addr.getSockaddr_in6(), &address_len);
        break;
    }
    }

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_socket::udp_socket_data::udp_socket_data() {
    sem_init(&semaphore, 0, 1);
    valid = false;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
network::udp_socket::udp_socket_data::~udp_socket_data() {
    sem_destroy(&semaphore);
    if (valid) {
        close(skt);
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool network::udp_socket::udp_socket_data::init(const uint16_t PORT, const int family) {
    sin_family = family;
    port = PORT;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    switch (sin_family) {
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    case AF_INET: {
        ipv4_addr local_address;
        local_address.init("", PORT);
        if ((skt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) != -1) {
            if (bind(skt, (sockaddr*)&local_address.getSockaddr_in(), sizeof(local_address.getSockaddr_in())) != -1) {
                valid = true;
            }
        }
        local_address.setValid(valid);
        break;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    case AF_INET6: {
        ipv6_addr local_address;
        local_address.init("", PORT);
        if ((skt = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP)) != -1) {
            if (bind(skt, (sockaddr*)&local_address.getSockaddr_in6(), sizeof(local_address.getSockaddr_in6())) != -1) {
                valid = true;
            }
        }
        local_address.setValid(valid);
        break;
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    }
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    return valid;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~



