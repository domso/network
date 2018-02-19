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
        // - blocking: set the SOCK_NONBLOCK options for the socket
        // Return:
        // - true  | on success
        // - false | on any error
        //______________________________________________________________________________________________________
        bool accept_on(const uint16_t port, const int backlog, const bool blocking = true) {
            // initialize the socket address
            this->m_addr.init("", port);

            // create new socket
            int socketType = SOCK_STREAM;
            if (!blocking) {
                socketType |= SOCK_NONBLOCK;
            }
            this->m_skt = socket(this->m_addr.family(), socketType, 0);

            if (this->m_skt == -1) {
                return false;
            }

            // set some further options
            int optval = 1;
            setsockopt(this->m_skt, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval , sizeof(int));

            // bind the socket to an address
            if (bind(this->m_skt, (struct sockaddr*) this->m_addr.internal_handle(), sizeof(*(this->m_addr.internal_handle()))) != 0) {
                return false;
            }
            
            unsigned int size = sizeof(*(this->m_addr.internal_handle()));
            if (getsockname(this->m_skt, (struct sockaddr*) this->m_addr.internal_handle(), &size) != 0) {
                return false;
            }

            // start listening for new incoming connections
            return listen(this->m_skt, backlog) == 0;
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - accepts new connections on the socket
        // - creates a new tcp_connection-object with the accepted socket
        // Return:
        // - smart-ptr to a new tcp_connection instance
        // - smart-ptr containing nullptr
        //_____________________________________________________________________________________________________
        std::shared_ptr<network::tcp_connection<IP_ADDR_TYPE>> accept_connection() const {
            // create object for new connection
            std::shared_ptr<network::tcp_connection<IP_ADDR_TYPE>> connection = std::make_shared<network::tcp_connection<IP_ADDR_TYPE>>();
            socklen_t clientlen = sizeof(*(connection->get_addr().internal_handle()));

            // accept new incoming connection
            int skt = accept(this->m_skt, (struct sockaddr*) connection->get_addr().internal_handle(), &clientlen);

            if (skt == -1) {
                return std::shared_ptr<network::tcp_connection<IP_ADDR_TYPE>>();
            }

            // combine socket and connection-object
            connection->set_socket(skt);

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
        bool accept_connection(network::tcp_connection<IP_ADDR_TYPE>& connection) const {
            socklen_t clientlen = sizeof(connection.get_addr().internal_handle());

            // accept new incoming connection
            int skt = accept(this->m_skt, (struct sockaddr*) connection.get_addr().internal_handle(), &clientlen);

            if (skt == -1) {
                return false;
            }

            // combine socket and connection-object
            connection.set_socket(skt);
            return true;
        }
    };
}

#endif
