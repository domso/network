#include "tcpServer.h"

void tcpServer::acceptConnection () {
    int status;
    if ( listen ( socketID, 5 ) == -1 ) {
        return;
    }
    while (true) {
        int new_sd;
        struct sockaddr_storage their_addr;
        socklen_t addr_size = sizeof ( their_addr );
        new_sd = accept ( socketID, ( struct sockaddr * ) &their_addr, &addr_size );
        if ( new_sd == -1 ) {
            return;
        } else {
            std::cout << "new connection" << new_sd <<  std::endl;
        }


    }
}

tcpServer::~tcpServer() {
    shutdown ( socketID, SHUT_RDWR );
}

void tcpServer::bindSocket() {
    int status;
    std::cout << "Binding socket..."  << std::endl;
    // we use to make the setsockopt() function to make sure the port is not in use
    // by a previous execution of our code. (see man page for more information)
    int yes = 1;
    status = setsockopt ( socketID, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof ( int ) );
    status = bind ( socketID, host_info_list->ai_addr, host_info_list->ai_addrlen );
    if ( status == -1 )  std::cout << "bind error" << std::endl ;
}
