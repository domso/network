#include <iostream>

#include <vector>
#include "unistd.h"


#include <chrono>

#include "semaphore.h"

#include "network/ip_addr.h"
#include "network/udp_socket.h"
#include "network/udp_receiver.h"
#include "network/ip_pkg.h"

#include "network/shared_queue.h"
#include "network/rw_mutex.h"

#include "network/rw_container.h"

int callBack_server (const network::ip_addr& addr, std::vector<char>& buffer, const int recvBytes, const network::udp_socket& socket ) {
    std::string tmp ( buffer.data() );
    std::cout << "Data: '" << tmp << "' (" << recvBytes << " Bytes) from " << inet_ntoa ( addr.getSockaddr_in().sin_addr ) << ":" << ntohs ( addr.getSockaddr_in().sin_port ) << std::endl;

    socket.send ( addr, buffer, recvBytes );
    return 0;
}

void callback(network::ip_pkg& pkg, const network::udp_socket& socket, const void* addPtr) {
   //std::cout << "callback:" + pkg.toString() << "||" << addPtr << std::endl;
	socket.send(pkg, 0);
}

void server() {
    network::udp_socket sock;

    if (!sock.init(5000, AF_INET)) {
        std::cout << "error while init socket!" << std::endl;

    }
    network::udp_receiver receiver;
    network::udp_receiver::udp_receiver_init_param parameter;

    parameter.sec2wait = 1;
    parameter.minThread = 4;
    parameter.maxThread = 4;
    parameter.addPtr = (void*)1234;
// callBack_server
    receiver.init ( sock, nullptr, &callback, &parameter);


    std::cin.get();
	
    receiver.stop();
}

int main ( int argc, char** argv ) {
     server();
}


