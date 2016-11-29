#include <vector>
#include<sys/socket.h>
#include<arpa/inet.h>
#include "ip_addr.h"
#include "ip_pkg.h"
#include "tcp_connection.h"

#include "base_socket.h"

namespace network {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //
    template<typename IP_ADDR_TYPE> 
    class tcp_socket : base_socket<IP_ADDR_TYPE> {
        public:
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            tcp_socket() {
                
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            ~tcp_socket() {
                
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            void listen(IP_ADDR_TYPE addr) {
                local_addr_ = addr;

                setSkt(socket(local_addr_.getFamily(), SOCK_STREAM, 0));

                int optval = 1;
                setsockopt(skt_, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));

                bind(skt_, (struct sockaddr *) &local_addr_.getSockaddr_in(), sizeof(local_addr_.getSockaddr_in()));
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            tcp_connection<IP_ADDR_TYPE>* accept() {
                tcp_connection<IP_ADDR_TYPE>* connection = new tcp_connection();
                int clientlen = sizeof(connection->getAddr().getSockaddr_in());              

                int skt = accept(skt_, (struct sockaddr *) &connection->getAddr().getSockaddr_in(), &clientlen);

                connection->skt_ = skt;
                connection->open();

                return connection;
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    };


}