#ifndef tcp_socket_h
#define tcp_socket_h

#include <memory>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "ip_addr.h"
#include "base_socket.h"
#include "tcp_connection.h"


namespace network {
    //______________________________________________________________________________________________________
    //
    // A wrapper for the classical tcp-accept-sockets
    //______________________________________________________________________________________________________
    template<typename IP_ADDR_TYPE>
    class tcp_socket : public network::base_socket<IP_ADDR_TYPE> {
    public:
        tcp_socket() {

        }
        tcp_socket(const tcp_socket& that) = delete;
        //______________________________________________________________________________________________________
        //
        // Description:
        // - closes the socket
        //______________________________________________________________________________________________________
        ~tcp_socket() {

        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - initializes a new tcp-socket
        // - binds the socket on an port
        // - starts listening for new connections
        // Parameters:
        // - port: the port on which the socket will be binded
        // - backlog: maximal number of pending connections
        // Return:
        // - true  | on success
        // - false | on any error
        //______________________________________________________________________________________________________
        bool acceptOn(const uint16_t port, int backlog) {
            // initialize the socket address
            this->addr_.init("", port);

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
        //______________________________________________________________________________________________________
        //
        // Description:
        // - accepts new connections on the socket
        // - creates a new tcp_connection-object with the accepted socket
        // - the object needs to be freed with 'destroy()' manually
        // Return:
        // - smart-ptr to a new tcp_connection instance
        // - smart-ptr containing nullptr
        //_____________________________________________________________________________________________________
        std::shared_ptr<network::tcp_connection<IP_ADDR_TYPE>> acceptConnection() const {
            // create object for new connection
            std::shared_ptr<network::tcp_connection<IP_ADDR_TYPE>> connection = std::make_shared<network::tcp_connection<IP_ADDR_TYPE>>();
            socklen_t clientlen = sizeof(*(connection->getAddr().getSockaddr_in()));

            // accept new incoming connection
            int skt = accept(this->skt_, (struct sockaddr*) connection->getAddr().getSockaddr_in(), &clientlen);

            if (skt == -1) {
                return std::shared_ptr<network::tcp_connection<IP_ADDR_TYPE>>();
            }

            // combine socket and connection-object
            connection->setSocket(skt);

            return connection;
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - accepts new connections on the socket
        // Parameter:
        // - connection: a valid noninitialized instance of tcp_connection in which the connection will be stored
        // Return:
        // - true if no error occured
        //______________________________________________________________________________________________________
        bool acceptConnection(network::tcp_connection<IP_ADDR_TYPE>& connection) const {
            socklen_t clientlen = sizeof(connection.getAddr().getSockaddr_in());

            // accept new incoming connection
            int skt = accept(this->skt_, (struct sockaddr*) connection.getAddr().getSockaddr_in(), &clientlen);

            if (skt == -1) {
                return false;
            }

            // combine socket and connection-object
            connection.setSocket(skt);
            return true;
        }
    };
}

#endif
