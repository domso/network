#ifndef udp_socket_h
#define udp_socket_h
#include <vector>
#include <mutex>
#include <unordered_set>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "semaphore.h"
#include "shared_queue.h"
#include "ip_addr.h"
#include "ip_pkg.h"

#include "base_socket.h"

namespace network {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // smartPointer-wrapper for an udp-socket
    template<typename IP_ADDR_TYPE> 
    class udp_socket : base_socket<IP_ADDR_TYPE>{
        public:
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // default constructor
            udp_socket(); {

            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // copy constructor
            udp_socket ( const udp_socket& that ) = delete;
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // default destructor
            ~udp_socket() {

            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // create a new udp socket and bind it to PORT
            bool init(const uint16_t PORT){
                local_addr_.init("", PORT);
                if ((skt_ = socket(local_addr_.getFamily(), SOCK_DGRAM, IPPROTO_UDP)) != -1) {
                     if (bind(skt_, (sockaddr*)&local_addr_.getSockaddr_in(), sizeof(local_addr_.getSockaddr_in())) != -1) {
                            open();
                            return true;
                        }
                }
                return false;
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // sends msglen bytes from buffer to address 
            // flags -> see 'man sendto()'
            // returns size of successfully send data
            ssize_t send(const IP_ADDR_TYPE& addr, const std::vector< char >& buffer, const int msglen, const int flags = 0 ) const {
                return sendto(skt_, buffer.data(), msglen, flags, (sockaddr*) &addr.getSockaddr_in(), sizeof(addr.getSockaddr_in()));
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // recvs data
            // -> returns size of recv-data
            // -> buffer: data will be stored here
            // -> address: address of the sender
            // -> flags: see 'man recvfrom()'
            ssize_t recv(const IP_ADDR_TYPE& addr, std::vector< char >& buffer, const int flags = 0 ) const {
                    socklen_t address_len = sizeof(addr.getSockaddr_in());
                    return recvfrom(skt, buffer.data(), buffer.size(), flags, (sockaddr*) &addr.getSockaddr_in(), &address_len);
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    };
}

#endif
