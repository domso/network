#ifndef udp_socket_h
#define udp_socket_h

#include <type_traits>
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
        // - blocking: set the SOCK_NONBLOCK option if false
        // Return:
        // - true  | on success
        // - false | on any error
        //______________________________________________________________________________________________________
        bool init(const uint16_t port, const bool blocking = true) {
            // initialize the socket address
            this->m_addr.init("", port);

            // create new socket
            int type = SOCK_DGRAM;
            
            if (!blocking) {
                type |= SOCK_NONBLOCK;
            }
            
            if ((this->m_skt = socket(this->m_addr.family(), type, IPPROTO_UDP)) != -1) {
                // bind the socket to the address
                unsigned int size = sizeof(*(this->m_addr.internal_handle()));
                return bind(this->m_skt, (sockaddr*)this->m_addr.internal_handle(), sizeof(*this->m_addr.internal_handle())) != -1
                    && getsockname(this->m_skt, (struct sockaddr*) this->m_addr.internal_handle(), &size) != -1;                
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
        // - <success, errno>
        //______________________________________________________________________________________________________
        template <typename MSG_DATA_TYPE>
        std::pair<bool, int> send_data(const IP_ADDR_TYPE& dest, const MSG_DATA_TYPE* buffer, const int numberOfData = 1, const int flags = 0) const {
            static_assert(std::is_trivially_copyable<MSG_DATA_TYPE>::value);
            int result = sendto(this->m_skt, buffer, sizeof(MSG_DATA_TYPE) * numberOfData, flags, (sockaddr*) dest.internal_handle(), sizeof(*dest.internal_handle())) / sizeof(MSG_DATA_TYPE);
            return this->check_error(result);
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
        // - <success, errno>
        //______________________________________________________________________________________________________
        template <typename MSG_DATA_TYPE>
        std::pair<bool, int> recv_data(IP_ADDR_TYPE& src, MSG_DATA_TYPE* buffer, const int numberOfData = 1, const int flags = 0) const {
            static_assert(std::is_trivially_copyable<MSG_DATA_TYPE>::value);
            socklen_t address_len = sizeof(*src.internal_handle());
            int result = recvfrom(this->m_skt, buffer, sizeof(MSG_DATA_TYPE) * numberOfData, flags, (sockaddr*) src.internal_handle(), &address_len) / sizeof(MSG_DATA_TYPE);
            return this->check_error(result);
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
        // - <success, errno>
        //______________________________________________________________________________________________________
        std::pair<bool, int> send_pkt(IP_ADDR_TYPE& dest, pkt_buffer& buffer, const int flags = 0) const {
            int result = sendto(this->m_skt, buffer.data(), buffer.msg_length(), flags, (sockaddr*) dest.internal_handle(), sizeof(*dest.internal_handle()));
            return this->check_error(result);
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
        // - <success, errno>
        //______________________________________________________________________________________________________
        std::pair<bool, int> recv_pkt(IP_ADDR_TYPE& src, pkt_buffer& buffer, const int flags = 0) const {            
            socklen_t address_len = sizeof(*src.internal_handle());
            int result = recvfrom(this->m_skt, buffer.data(), buffer.capacity(), flags, (sockaddr*) src.internal_handle(), &address_len);
            if (result == -1) {
                buffer.set_msg_length(0);
            } else {
                buffer.set_msg_length(result);
            }
            
            return this->check_error(result);
        }
    };
}

#endif
