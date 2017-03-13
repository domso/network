#ifndef udp_socket_h
#define udp_socket_h

#include <sys/socket.h>
#include <arpa/inet.h>
#include "ip_addr.h"
#include "base_socket.h"
#include "pkt_buffer.h"

namespace network {
    //______________________________________________________________________________________________________
    //
    // A wrapper for the classical udp-sockets
    //______________________________________________________________________________________________________
    template<typename IP_ADDR_TYPE>
    class udp_socket : public base_socket<IP_ADDR_TYPE> {
        public:
            udp_socket() {

            }

            udp_socket(const udp_socket& that) = delete;
            //______________________________________________________________________________________________________
            //
            // Description:
            // - closes the socket
            //______________________________________________________________________________________________________
            ~udp_socket() {

            }
            //______________________________________________________________________________________________________
            //
            // Description:
            // - creates a new udp socket and bind it to port
            // Parameter:
            // - port: application-port for the udp-protocol on which the new socket will be binded
            // Return:
            // - true  | on success
            // - false | on any error
            //______________________________________________________________________________________________________
            bool init(const uint16_t port) {
                // initialize the socket address
                this->addr_.init("", port);
                // create new socket
                if ((this->skt_ = socket(this->addr_.getFamily(), SOCK_DGRAM, IPPROTO_UDP)) != -1) {
                    // bind the socket to the address
                    if (bind(this->skt_, (sockaddr*)this->addr_.getSockaddr_in(), sizeof(*this->addr_.getSockaddr_in())) != -1) {
                        // mark the socket as open
                        this->open();
                        return true;
                    }
                }
                return false;
            }
            //______________________________________________________________________________________________________
            //
            // Description:
            // - sends numberOfData*sizeof(MSG_DATA_TYPE) Bytes starting from buffer to the destination-address
            // Parameters:
            // - dest: destination-address
            // - buffer: pointer to atleast one valid MSG_DATA_TYPE-instance
            // - numberOfData: number of instances of MSG_DATA_TYPE | 1 on default
            // - flags: see 'man sendto()'
            // Return:
            // - number of successfully send MSG_DATA_TYPE-objects
            // - -1 on any error
            //______________________________________________________________________________________________________
            template <typename MSG_DATA_TYPE>
            ssize_t sendData(const IP_ADDR_TYPE& dest, const MSG_DATA_TYPE* buffer, const int numberOfData = 1, const int flags = 0) const {
                return sendto(this->skt_, buffer, sizeof(MSG_DATA_TYPE) * numberOfData, flags, (sockaddr*) dest.getSockaddr_in(), sizeof(*dest.getSockaddr_in())) / sizeof(MSG_DATA_TYPE);
            }
            //______________________________________________________________________________________________________
            //
            // Description:
            // - receives maximal numberOfData*sizeof(MSG_DATA_TYPE) Bytes from any source-address and stores them into the buffer
            // Parameters:
            // - src: source-address
            // - buffer: pointer to atleast one valid MSG_DATA_TYPE-instance
            // - numberOfData: number of instances of MSG_DATA_TYPE | 1 on default
            // - flags: see 'man recvfrom()'
            // Return:
            // - number of successfully received MSG_DATA_TYPE-objects
            // - -1 on any error
            //______________________________________________________________________________________________________
            template <typename MSG_DATA_TYPE>
            ssize_t recvData(IP_ADDR_TYPE& src, MSG_DATA_TYPE* buffer, const int numberOfData = 1, const int flags = 0) const {
                socklen_t address_len = sizeof(*src.getSockaddr_in());
                return recvfrom(this->skt_, buffer, sizeof(MSG_DATA_TYPE) * numberOfData, flags, (sockaddr*) src.getSockaddr_in(), &address_len) / sizeof(MSG_DATA_TYPE);
            }
            //______________________________________________________________________________________________________
            //
            // Description:
            // - sends buffer.msgLen()-Bytes from the buffer-content to the destination-address
            // Parameters:
            // - dest: destination-address
            // - buffer: reference to a pkt_buffer with a msgLen > 0
            // - flags: see 'man sendto()'
            // Return:
            // - size of successfully send data
            // - -1 on any error
            //______________________________________________________________________________________________________
            size_t sendPkt(IP_ADDR_TYPE& dest, pkt_buffer& buffer, const int flags = 0) const {
                return sendto(this->skt_, buffer.data(), buffer.msgLen(), flags, (sockaddr*) dest.getSockaddr_in(), sizeof(*dest.getSockaddr_in()));
            }
            //______________________________________________________________________________________________________
            //
            // Description:
            // - receives maximal buffer.capacity-Bytes from any source-address and stores them into the buffer
            // Parameters:
            // - src: source-address
            // - buffer: reference to a pkt_buffer
            // - flags: see 'man recvfrom()'
            // Return:
            // - size of successfully received data
            // - -1 on any error
            //______________________________________________________________________________________________________
            ssize_t recvPkt(IP_ADDR_TYPE& src, pkt_buffer& buffer, const int flags = 0) const {
                socklen_t address_len = sizeof(*src.getSockaddr_in());
                int result = recvfrom(this->skt_, buffer.data(), buffer.capacity(), flags, (sockaddr*) src.getSockaddr_in(), &address_len);;
                if (result < 1) {
                    buffer.setMsgLen(0);
                    result = ((errno == EAGAIN) & (result == -1)) - 1;
                }
                return result;
            }
    };
}

#endif
