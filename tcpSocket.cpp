#include "tcpSocket.h"


tcpSocket::tcpSocket (  ) {

    int status;
    memset ( &host_info, 0, sizeof host_info );
    setUpHostInfo();
    status = getaddrinfo ( NULL, "5556", &host_info, &host_info_list );
    if (status != 0)  std::cout << "getaddrinfo error" << gai_strerror(status) ;
    socketID = socket ( host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol );
}

tcpSocket::~tcpSocket() {
    closeSockets();
}

void tcpSocket::setUpHostInfo() {
    host_info.ai_family = AF_UNSPEC;     // IP version not specified. Can be both.
    host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.
    host_info.ai_flags = AI_PASSIVE;                          // IP Wildcard
}

void tcpSocket::closeSockets() {
    close ( socketID );
    freeaddrinfo ( host_info_list );
}