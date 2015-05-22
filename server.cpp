#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include "unistd.h"
#include "thread.h"
#include "server.h"
#include "swapVector.h"
#include <chrono>

tcpSocket::tcpSocket ( std::string IP ) {

    int status;
    memset ( &host_info, 0, sizeof host_info );
    setUpHostInfo();
    status = getaddrinfo ( NULL, "5556", &host_info, &host_info_list );
    //if (status != 0)  std::cout << "getaddrinfo error" << gai_strerror(status) ;
    socketID = socket ( host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol );
}

tcpSocket::~tcpSocket() {
    closeSockets();
    std::cout << "parent" << std::endl;
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

void tcpServer::acceptConnection ( threadController<int> * controller ) {
    int status;
    if ( listen ( socketID, 5 ) == -1 ) {
        return;
    }
    while ( controller->isRunning() ) {
        int new_sd;
        struct sockaddr_storage their_addr;
        socklen_t addr_size = sizeof ( their_addr );
        new_sd = accept ( socketID, ( struct sockaddr * ) &their_addr, &addr_size );
        if ( new_sd == -1 ) {
            return;
        } else {
            std::cout << "new connection" << new_sd <<  std::endl;
            recvController.send_input ( new_sd );
        }


    }
}

tcpServer::~tcpServer() {
    shutdown ( socketID, SHUT_RDWR );
    acceptConnectionController.stop();
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


void tcpServer::startAccept() {
    acceptConnectionController.setThread ( new std::thread ( &tcpServer::acceptConnection, this, &acceptConnectionController ) );
}






void tcpServer::recvConnection ( threadController<int>* controller ) {
    if ( controller == 0 ) return;
    useThreadController ( int, controller );
    threadController<int> RightChildRecvController;
    threadController<int> LeftChildRecvController;
    int modChanger = 0;

    std::vector<char> databuffer ( 1000, 0 );

    int countRecv = 0;
    swapVector<int> socketContainer;

    double lastIterationTime = -1;
    double maxIterationTime = 0.001;
    double fractionLoad = 0.5;
    int kickSocketCount = 0;
    bool lockIO = false;

    int socketIndex = 0;
    int i = 0;
    int tmp = 0;
    while ( controller->isRunning() ) {
        std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

        if ( controller->try_lock_IO() ) {
            while ( tmp = controller->recv_input ( false ) ) {
                socketContainer.push ( tmp );
            }
            controller->unlock_IO();
        }
        if ( lockIO ) {
            RightChildRecvController.lock_IO();
            LeftChildRecvController.lock_IO();
        }

        socketIndex = 0;
        while ( socketIndex < socketContainer.size() ) {
            sleep ( 1 );
            std::cout << "read " << socketIndex << std::endl;
            countRecv = recv ( socketContainer[socketIndex], databuffer.data(), 1000, 0);
            if ( countRecv > 0 ) {
                std::cout << "receive something!" << std::endl;

                if ( kickSocketCount > 0 ) {
                    if ( modChanger == 0 ) {
                        RightChildRecvController.send_input ( socketContainer[socketIndex], false );
                    } else {
                        LeftChildRecvController.send_input ( socketContainer[socketIndex], false );
                    }
                    modChanger = ( modChanger + 1 ) % 2;
                    socketContainer.remove ( socketIndex );
                    kickSocketCount--;
                } else {
                    socketIndex++;
                }
            } else {
                socketContainer.remove ( socketIndex );
                if ( kickSocketCount > 0 ) kickSocketCount--;
            }
        }
        if ( lockIO ) {
            RightChildRecvController.unlock_IO();
            LeftChildRecvController.unlock_IO();
        }
        std::chrono::high_resolution_clock::time_point endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>> ( endTime - startTime );
        if ( time_span.count() > maxIterationTime && ( 1 + 1 == 3 ) ) {
            kickSocketCount = socketContainer.size() * fractionLoad;
            lockIO = true;
            if ( RightChildRecvController.isRunning() == false ) {
                RightChildRecvController.setThread ( new std::thread ( &tcpServer::recvConnection,&RightChildRecvController ) );
            }
            if ( LeftChildRecvController.isRunning() == false ) {
                LeftChildRecvController.setThread ( new std::thread ( &tcpServer::recvConnection,&LeftChildRecvController ) );
            }
        } else {
            lockIO = false;
        }
    }
}


void tcpServer::startRecv() {
    recvController.setThread ( new std::thread ( &tcpServer::recvConnection, &recvController ) );
}



void tcpClient::connectSocket() {
    int status;
    status = connect ( socketID, host_info_list->ai_addr, host_info_list->ai_addrlen );
    if ( status == -1 )  std::cout << "connect error" ;


}
