#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "ip_addr.h"
#include "tcp_connection.h"

#include "errno.h"

#include "base_socket.h"

namespace network {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //
    template<typename IP_ADDR_TYPE>
    class tcp_socket : public network::base_socket<IP_ADDR_TYPE> {
        public:
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            tcp_socket() {

            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            ~tcp_socket() {

            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            bool acceptOn(IP_ADDR_TYPE addr, int backlog) {
                this->addr_ = addr;

                this->skt_ = socket(this->addr_.getFamily(), SOCK_STREAM, 0);

                if (this->skt_ == -1) {
                    return false;
                }

                int optval = 1;
                setsockopt(this->skt_, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval , sizeof(int));

                if (bind(this->skt_, (struct sockaddr*) this->addr_.getSockaddr_in(), sizeof(*(this->addr_.getSockaddr_in()))) != 0) {
                    return false;
                }

                return listen(this->skt_, backlog) == 0;
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            tcp_connection<IP_ADDR_TYPE>* acceptConnection() {
                network::tcp_connection<IP_ADDR_TYPE>* connection = new network::tcp_connection<IP_ADDR_TYPE>();
                socklen_t clientlen = sizeof(*(connection->getAddr().getSockaddr_in()));

                int skt = accept(this->skt_, (struct sockaddr*) connection->getAddr().getSockaddr_in(), &clientlen);
                if (skt == -1) {
                    return nullptr;
                }

                connection->setSocket(skt);
                connection->open();

                return connection;
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    };


}
