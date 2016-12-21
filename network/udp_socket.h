#ifndef udp_socket_h
#define udp_socket_h
#include <vector>
#include <mutex>
#include <unordered_set>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "ip_addr.h"

#include "base_socket.h"
#include "pkt_buffer.h"


namespace network {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    //
    template<typename IP_ADDR_TYPE> 
    class udp_socket : base_socket<IP_ADDR_TYPE>{
        public:
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // default constructor
            udp_socket() {

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
                this->addr.init("", PORT);
                if ((this->skt_ = socket(this->addr.getFamily(), SOCK_DGRAM, IPPROTO_UDP)) != -1) {
                     if (bind(this->skt_, (sockaddr*)&this->addr.getSockaddr_in(), sizeof(this->addr.getSockaddr_in())) != -1) {
                            this->open();
                            return true;
                        }
                }
                return false;
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // sends msglen bytes from buffer to address 
            // flags -> see 'man sendto()'
            // returns size of successfully send data
            ssize_t sendData(const IP_ADDR_TYPE& addr, const char* buffer, const int msglen, const int flags = 0 ) const {
                return sendto(this->skt_, buffer, msglen, flags, (sockaddr*) &addr.getSockaddr_in(), sizeof(addr.getSockaddr_in()));
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // recvs data
            // -> returns size of recv-data
            // -> buffer: data will be stored here
            // -> address: address of the sender
            // -> flags: see 'man recvfrom()'
            ssize_t recvData(IP_ADDR_TYPE& addr,char* buffer,const int bufferSize, const int flags = 0 ) const {
                    socklen_t address_len = sizeof(addr.getSockaddr_in());
                    return recvfrom(this->skt_, buffer, bufferSize, flags, (sockaddr*) &addr.getSockaddr_in(), &address_len);
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // sends msglen bytes from buffer to address 
            // flags -> see 'man sendto()'
            // returns size of successfully send data
            bool sendData(const pkt_buffer<IP_ADDR_TYPE>& buffer, const int flags = 0 ) const {
                return buffer.msgLen == sendto(this->skt_, buffer.msg_buffer.data(), buffer.msg_buffer.size(), flags, (sockaddr*) &buffer.addr.getSockaddr_in(), sizeof(buffer.addr.getSockaddr_in()));
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // recvs data
            // -> returns size of recv-data
            // -> buffer: data will be stored here
            // -> address: address of the sender
            // -> flags: see 'man recvfrom()'
            ssize_t recvData(pkt_buffer<IP_ADDR_TYPE>& buffer, const int flags = 0 ) const {
                    socklen_t address_len = sizeof(buffer.addr.getSockaddr_in());
                    buffer.msgLen = recvfrom(this->skt_, buffer.msg_buffer.data(), buffer.msg_buffer.size(), flags, (sockaddr*) &buffer.addr.getSockaddr_in(), &address_len);
                    return buffer.msgLen;
            }            
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    };
}

#endif
