#include <iostream>

#include <vector>
#include "unistd.h"

#include "network/udp_receiver.h"
#include "network/ip_pkg.h"

#include "network/rw_mutex.h"

#include "network/rw_container.h"

#define NUM_PKG 1000000

void callback(network::ip_pkg& pkg, const network::udp_socket& socket, const void* addPtr) {
	if (pkg.compare("ADDR")) {
        std::cout << "asd" << std::endl;
     }
}

void client() {
    network::udp_socket sock;
    network::udp_receiver receiver;
    
    if ( !sock.init (5002, AF_INET) ) {
        std::cout << "error while init socket!" << std::endl;

    }
    
    receiver.init (sock, callback,nullptr);

    network::ipv4_addr partner;
    network::ipv4_addr recv_addr;
    partner.init("85.25.192.29", 5000 );

    sock.send(partner, "CON");
    
    std::cin.get();
    
    sock.send(partner, "FIN");
    receiver.stop();
}

int main ( int argc, char** argv ) {
     client();
}


