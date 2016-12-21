#ifndef tcp_connection_h
#define tcp_connection_h

#include <sys/socket.h>
#include <arpa/inet.h>

#include "ip_addr.h"
#include "base_socket.h"
#include "pkt_buffer.h"


namespace network {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // A wrapper for the classical tcp-connection-sockets
    template<typename IP_ADDR_TYPE>
    class tcp_connection : public base_socket<IP_ADDR_TYPE> {
        public:
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            tcp_connection() {

            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            tcp_connection(const tcp_connection& that) = delete;
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Description:
            // - closes the socket
            ~tcp_connection() {

            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Description:
            // - sends numberOfData*sizeof(MSG_DATA_TYPE) Bytes starting from buffer
            // Parameters:
            // - buffer: pointer to atleast one valid MSG_DATA_TYPE-instance
            // - numberOfData: number of instances of MSG_DATA_TYPE | 1 on default
            // - flags: see 'man send()'
            // Return:
            // - size of successfully send data
            template <typename MSG_DATA_TYPE>
            ssize_t sendData(const MSG_DATA_TYPE* buffer, const int numberOfData, const int flags = 0) const {
                return send(this->skt_, buffer, sizeof(MSG_DATA_TYPE) * numberOfData, flags);
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Description:
            // - receives maximal numberOfData*sizeof(MSG_DATA_TYPE) Bytes and stores them into the buffer
            // Parameters:
            // - buffer: pointer to atleast one valid MSG_DATA_TYPE-instance
            // - numberOfData: number of instances of MSG_DATA_TYPE | 1 on default
            // - flags: see 'man recv()'
            // Return:
            // - size of successfully received data
            template <typename MSG_DATA_TYPE>
            ssize_t recvData(MSG_DATA_TYPE* buffer, const int numberOfData, const int flags = 0) const {
                return recv(this->skt_, buffer, sizeof(MSG_DATA_TYPE) * numberOfData, flags);
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Description:
            // - sends buffer.msgLen()-Bytes from the buffer-content
            // Parameters:
            // - buffer: reference to a pkt_buffer with a msgLen > 0
            // - flags: see 'man send()'
            // Return:
            // - size of successfully send data
            ssize_t sendPkt(pkt_buffer& buffer, const int flags = 0) const {
                return send(this->skt_, buffer.data(), buffer.msgLen(), flags);
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // Description:
            // - receives maximal buffer.capacity-Bytes and stores them into the buffer
            // Parameters:
            // - buffer: reference to a pkt_buffer
            // - flags: see 'man recv()'
            // Return:
            // - size of successfully received data
            ssize_t recvPkt(pkt_buffer& buffer, const int flags = 0) const {
                buffer.setMsgLen(recv(this->skt_, buffer.data(), buffer.capacity(), flags));
                return buffer.msgLen();
            }
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    };

}

#endif
