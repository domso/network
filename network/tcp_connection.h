#ifndef tcp_connection_h
#define tcp_connection_h

#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include "ip_addr.h"
#include "base_socket.h"
#include "pkt_buffer.h"

namespace network {
    //______________________________________________________________________________________________________
    //
    // A wrapper for the classical tcp-connection-sockets
    //______________________________________________________________________________________________________
    template<typename IP_ADDR_TYPE>
    class tcp_connection : public base_socket<IP_ADDR_TYPE> {
    public:
        tcp_connection() {

        }
        tcp_connection(const tcp_connection& that) = delete;
        tcp_connection(tcp_connection&& conn) : base_socket<IP_ADDR_TYPE>(std::move(conn)) {

        }
        tcp_connection& operator=(tcp_connection && conn) {
            base_socket<IP_ADDR_TYPE>::operator=(std::move(conn));

            return *this;
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - closes the socket
        //______________________________________________________________________________________________________
        ~tcp_connection() {

        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - tries to open a connection to the host specified by 'addr'
        // Parameter:
        // - addr: host-address
        // Return:
        // - true  | on success
        // - false | on error
        //______________________________________________________________________________________________________
        bool connect_to(const IP_ADDR_TYPE& addr) {
            this->m_addr = addr;

            // create new socket
            this->m_skt = socket(this->m_addr.family(), SOCK_STREAM, 0);

            if (this->m_skt == -1) {
                return false;
            }

            if (connect(this->m_skt, (struct sockaddr*) this->m_addr.internal_handle(), sizeof(*(this->m_addr.internal_handle()))) != 0) {
                return false;
            }

            return true;
        }

        //______________________________________________________________________________________________________
        //
        // Description:
        // - sends numberOfData*sizeof(MSG_DATA_TYPE) Bytes starting from buffer
        // Parameters:
        // - buffer: pointer to atleast one valid MSG_DATA_TYPE-instance
        // - numberOfData: number of instances of MSG_DATA_TYPE | 1 on default
        // - flags: see 'man send()'
        // Return:
        // - size of successfully send data
        //______________________________________________________________________________________________________
        template <typename MSG_DATA_TYPE>
        ssize_t send_data(const MSG_DATA_TYPE* buffer, const int numberOfData, const int flags = 0) const {
            return send(this->m_skt, buffer, sizeof(MSG_DATA_TYPE) * numberOfData, flags);
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - receives maximal numberOfData*sizeof(MSG_DATA_TYPE) Bytes and stores them into the buffer
        // Parameters:
        // - buffer: pointer to atleast one valid MSG_DATA_TYPE-instance
        // - numberOfData: number of instances of MSG_DATA_TYPE | 1 on default
        // - flags: see 'man recv()'
        // Return:
        // - size of successfully received data
        //______________________________________________________________________________________________________
        template <typename MSG_DATA_TYPE>
        ssize_t recv_data(MSG_DATA_TYPE* buffer, const int numberOfData, const int flags = 0) const {
            return recv(this->m_skt, buffer, sizeof(MSG_DATA_TYPE) * numberOfData, flags);
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - sends buffer.msgLen()-Bytes from the buffer-content
        // Parameters:
        // - buffer: reference to a pkt_buffer with a msgLen > 0
        // - flags: see 'man send()'
        // Return:
        // - size of successfully send data
        //______________________________________________________________________________________________________
        ssize_t send_pkt(pkt_buffer& buffer, const int flags = 0) const {
            return send(this->m_skt, buffer.data(), buffer.msg_length(), flags);
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - receives maximal buffer.capacity-Bytes and stores them into the buffer
        // Parameters:
        // - buffer: reference to a pkt_buffer
        // - flags: see 'man recv()'
        // Return:
        // - size of successfully received data
        // - -1 on any error
        //______________________________________________________________________________________________________
        int recv_pkt(pkt_buffer& buffer, const int flags = 0) const {
            int result = recv(this->m_skt, buffer.data(), buffer.capacity(), flags);

            if (result < 1) {
                buffer.set_msg_length(0);
                result = ((errno == EAGAIN) & (result == -1)) - 1;
            } else {
                buffer.set_msg_length(result);
            }

            return result;
        }
    };
}

#endif
