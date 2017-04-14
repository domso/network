#ifndef tcp_connection_h
#define tcp_connection_h

#include <sys/socket.h>
#include <arpa/inet.h>
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
            bool connectTo(const IP_ADDR_TYPE& addr){
                this->addr_ = addr;
                
                // create new socket
                this->skt_ = socket(this->addr_.getFamily(), SOCK_STREAM, 0);
                if (this->skt_ == -1) {
                    return false;
                }
                
                if (connect(this->skt_, (struct sockaddr*) this->addr_.getSockaddr_in(), sizeof(*(this->addr_.getSockaddr_in()))) != 0) {
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
            ssize_t sendData(const MSG_DATA_TYPE* buffer, const int numberOfData, const int flags = 0) const {
                return send(this->skt_, buffer, sizeof(MSG_DATA_TYPE) * numberOfData, flags);
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
            ssize_t recvData(MSG_DATA_TYPE* buffer, const int numberOfData, const int flags = 0) const {
                return recv(this->skt_, buffer, sizeof(MSG_DATA_TYPE) * numberOfData, flags);
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
            ssize_t sendPkt(pkt_buffer& buffer, const int flags = 0) const {
                return send(this->skt_, buffer.data(), buffer.msgLen(), flags);
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
            //______________________________________________________________________________________________________
            ssize_t recvPkt(pkt_buffer& buffer, const int flags = 0) const {
                buffer.setMsgLen(recv(this->skt_, buffer.data(), buffer.capacity(), flags));
                return buffer.msgLen();
            }
    };
}

#endif
