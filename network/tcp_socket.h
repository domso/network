#ifndef tcp_socket_h
#define tcp_socket_h

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "ip_addr.h"
#include "base_socket.h"
#include "tcp_connection.h"


namespace network {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // A wrapper for the classical tcp-accept-sockets
    template<typename IP_ADDR_TYPE>
    class tcp_socket : public network::base_socket<IP_ADDR_TYPE> {
        public:
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            tcp_socket() {

            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Description:
            // - closes the socket
            ~tcp_socket() {

            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Description:
            // - initializes a new tcp-socket
            // - binds the socket on an address
            // - starts listening for new connections
            // Parameters:
            // - addr: the address on which the socket will be binded
            // - backlog: maximal number of pending connections
            // Return:
            // - true  | on success
            // - false | on any error
            bool acceptOn(IP_ADDR_TYPE addr, int backlog) {
                this->addr_ = addr;

                // create new socket
                this->skt_ = socket(this->addr_.getFamily(), SOCK_STREAM, 0);
                if (this->skt_ == -1) {
                    return false;
                }

                // set some further options
                int optval = 1;
                setsockopt(this->skt_, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval , sizeof(int));

                // bind the socket to an address
                if (bind(this->skt_, (struct sockaddr*) this->addr_.getSockaddr_in(), sizeof(*(this->addr_.getSockaddr_in()))) != 0) {
                    return false;
                }

                // start listening for new incoming connections
                return listen(this->skt_, backlog) == 0;
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Description:
            // - accepts new connections on the socket
            // - creates a new tcp_connection-object with the accepted socket
            // - the object needs to be freed with 'destroy()' manually
            // Return:
            // - pointer to a new tcp_connection instance
            tcp_connection<IP_ADDR_TYPE>* acceptConnection() {
                // create object for new connection
                network::tcp_connection<IP_ADDR_TYPE>* connection = new network::tcp_connection<IP_ADDR_TYPE>();
                socklen_t clientlen = sizeof(*(connection->getAddr().getSockaddr_in()));

                // accept new incoming connection
                int skt = accept(this->skt_, (struct sockaddr*) connection->getAddr().getSockaddr_in(), &clientlen);
                if (skt == -1) {
                    return nullptr;
                }

                // combine socket and connection-object
                connection->setSocket(skt);
                // mark the socket as open
                connection->open();

                return connection;
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    };


}

#endif
