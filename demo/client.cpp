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

#define NUM_PKG 1000000

int callBack_client (const network::ip_addr& addr, std::vector<char>& buffer, const int recvBytes, const network::udp_socket& socket ) {
    std::string tmp ( buffer.data() );
    std::cout << "Data: '" << tmp << "' (" << recvBytes << " Bytes) from " << inet_ntoa ( addr.getSockaddr_in().sin_addr ) << ":" << ntohs ( addr.getSockaddr_in().sin_port ) << std::endl;
    return -1;
}
void callback(network::ip_pkg& pkg, const network::udp_socket& socket, const void* addPtr) {
	std::cout << "callback:" << std::endl;
}

void client() {
    network::udp_socket sock;

    if ( !sock.init (5002, AF_INET) ) {
        std::cout << "error while init socket!" << std::endl;

    }
/*
    network::udp_receiver receiver;
    network::udp_receiver::udp_receiver_init_param parameter;

    parameter.sec2wait = 1;
    parameter.minThread = 4;
    parameter.maxThread = 4;

    receiver.init (sock, nullptr, &callback, &parameter);
*/
    network::ipv4_addr partner;
    network::ipv4_addr recv_addr;
    partner.init("127.0.0.1", 5000 );

    std::vector<char> buffer_recv ( 500, '\0' );

    std::vector<char> buffer_send ( 500, '\0' );

    for (int i = 0 ; i < 100 ; i++) {
        buffer_send[i] = 65 + (i % 18);
    }

    sock.setTimeout(1);
    double summe=0;
    double N=0;
	std::chrono::high_resolution_clock::time_point t1;
	std::chrono::high_resolution_clock::time_point t2;
    for (int i = 0 ; i < NUM_PKG ; i++) {
	t1 = std::chrono::high_resolution_clock::now();
        sock.send(partner, buffer_send, 100);
	if (sock.recv(recv_addr, buffer_recv, 0) > 0){
	    t2 = std::chrono::high_resolution_clock::now();
	    summe += (double)std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
	    N++;
	}

    }
    std::cout << "time:" << summe << std::endl;
    std::cout << "pkg count:" << N << std::endl;
    std::cout << summe/N << std::endl;
	std::cin.get();

}

int main ( int argc, char** argv ) {
     client();
}


