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
int callBack_client (const network::ip_addr& addr, std::vector<char>& buffer, const int recvBytes, const network::udp_socket& socket ) {
    std::string tmp ( buffer.data() );
    std::cout << "Data: '" << tmp << "' (" << recvBytes << " Bytes) from " << inet_ntoa ( addr.getSockaddr_in().sin_addr ) << ":" << ntohs ( addr.getSockaddr_in().sin_port ) << std::endl;
    return -1;
}
int t = 0;
void callback(network::ip_pkg& pkg, const network::udp_socket& socket, const void* addPtr) {
   std::cout << "callback:" + pkg.toString() << "||" << addPtr << std::endl;
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

void client() {
    network::udp_socket sock;

    if ( !sock.init (5002, AF_INET) ) {
        std::cout << "error while init socket!" << std::endl;

    }
    network::udp_receiver receiver;

    receiver.init (sock, &callBack_client, &callback, 0);

    network::ipv4_addr partner;
    partner.init("91.60.124.103", 5000 );

    std::vector<char> buffer ( 500, '\0' );

    buffer[0] = 'H';
    buffer[1] = 'A';
    buffer[2] = 'L';
    buffer[3] = 'L';
    buffer[4] = 'O';
    buffer[5] = '!';

    for (int i = 0 ; i < 1000 ; i++) {
        buffer[0] = 65 + (i % 18);
        buffer[1] = 66 + (i % 18);
        buffer[2] = 67 + (i % 18);
        buffer[3] = 68 + (i % 18);
        buffer[4] = 69 + (i % 18);
        buffer[5] = 70 + (i % 18);
        sock.send ( partner, buffer);
    }
//std::cin.get();
    receiver.stop();

}

int main ( int argc, char** argv ) {
     server();
//     client();
}


