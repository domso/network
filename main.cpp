#include <iostream>
#include "thread.h"
#include <vector>
#include "unistd.h"
#include "server.h"
#include "msgLog.h"

#include <chrono>

#include "semaphore.h"

#include "simpleList.h"

#include "ip_addr.h"
#include "udp_socket.h"
#include "udp_receiver.h"
#include "ip_pkg.h"

#include "udp_worker.h"

#include "shared_queue.h"


int callBack_server ( network::ip_addr& addr,std::vector<char>& buffer,int recvBytes,network::udp_socket socket ) {
    std::string tmp ( buffer.data() );
    std::cout << "Data: '" << tmp << "' (" << recvBytes << " Bytes) from " << inet_ntoa ( addr.getSockaddr_in().sin_addr ) << ":" << ntohs ( addr.getSockaddr_in().sin_port ) << std::endl;

    socket.send ( addr,buffer,recvBytes );
    return 0;
}
int callBack_client ( network::ip_addr& addr,std::vector<char>& buffer,int recvBytes,network::udp_socket socket ) {
    std::string tmp ( buffer.data() );
    std::cout << "Data: '" << tmp << "' (" << recvBytes << " Bytes) from " << inet_ntoa ( addr.getSockaddr_in().sin_addr ) << ":" << ntohs ( addr.getSockaddr_in().sin_port ) << std::endl;
    return -1;
}

void callback(network::ip_pkg pkg,network::udp_socket socket){
	std::cout << "callback:" + pkg.toString() << std::endl;
}

void server() {
    network::udp_socket sock;

    if ( !sock.init ( 5000 ) ) {
        std::cout << "error while init socket!" << std::endl;

    }
    network::udp_receiver receiver;

    receiver.init ( sock,1000,&callBack_server,&callback,1,1);

    std::cin.get();
	receiver.stop();
}

void client() {
    network::udp_socket sock;

    if ( !sock.init ( 5002 ) ) {
        std::cout << "error while init socket!" << std::endl;

    }
    network::udp_receiver receiver;

    receiver.init ( sock,1000,&callBack_client,&callback, 1, 1);

    network::ip_addr partner;
    partner.init ( "127.0.0.1",5000 );

    std::vector<char> buffer ( 500,'\0' );

    buffer[0] = 'H';
    buffer[1] = 'A';
    buffer[2] = 'L';
    buffer[3] = 'L';
    buffer[4] = 'O';


    sock.send ( partner,buffer,5);
	receiver.stop();
// std::cin.get();
}


int main ( int argc, char **argv ) {
//     server();
    client();
		
}


