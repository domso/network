#ifndef tcp_connection_h
#define tcp_connection_h

#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <cstring>
#include "ip_addr.h"
#include "base_socket.h"
#include "pkt_buffer.h"

namespace network {
/**
* @brief A wrapper for the classical tcp-conneaction-sockets
*
*/
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

    /**
    * @brief closes the socket
    *
    */
    ~tcp_connection() {

    }

    /**
    * @brief tries to open a connection to the host specified by @addr
    *
    * @param addr host-address
    * @return bool: success
    */
    bool connect_to(const IP_ADDR_TYPE& addr) {
        this->m_addr = addr;

        // create new socket
        this->m_skt = socket(this->m_addr.family(), SOCK_STREAM, 0);

        if (this->m_skt == -1) {
            return false;
        }

        if (connect(
                    this->m_skt,
                    (struct sockaddr*) this->m_addr.internal_handle(),
                    sizeof(*(this->m_addr.internal_handle()))) != 0
           ) {
            return false;
        }

        return true;
    }

    /**
    * @brief sends numberOfData * sizeof(MSG_DATA_TYPE) Bytes to the connected socket
    *
    * @param MSG_DATA_TYPE type of the data
    * @param buffer buffer containing the data
    * @param numberOfData number of elements in the buffer
    * @param flags see 'man send()'
    * @return {success, errno}
    */
    template <typename MSG_DATA_TYPE>
    std::pair<bool, int> send_data(const MSG_DATA_TYPE* buffer, const int numberOfData, const int flags = MSG_NOSIGNAL) const {
        static_assert(std::is_trivially_copyable<MSG_DATA_TYPE>::value);
        size_t offset = 0;
        int result = 0;
        
        while (result >= 0 && offset < (sizeof(MSG_DATA_TYPE) * numberOfData)) {
            result = send(this->m_skt, buffer + offset, sizeof(MSG_DATA_TYPE) * (numberOfData - offset), flags);
            offset += result;
        }       
        
        return this->check_error(result);
    }

    /**
    * @brief receives maximal numberOfData * sizeof(MSG_DATA_TYPE) Bytes and stores them into the buffer
    *
    * @param MSG_DATA_TYPE type of the data
    * @param buffer destination buffer for new data
    * @param numberOfData size of buffer
    * @param flags see 'man recv()'
    * @return {success, errno}
    */
    template <typename MSG_DATA_TYPE>
    std::pair<bool, int> recv_data(MSG_DATA_TYPE* buffer, const int numberOfData, const int flags = 0) const {
        static_assert(std::is_trivially_copyable<MSG_DATA_TYPE>::value);
        int result = recv(this->m_skt, buffer, sizeof(MSG_DATA_TYPE) * numberOfData, flags);
        return this->check_error(result);
    }

    /**
    * @brief sends buffer.msgLen()-Bytes
    *
    * @param buffer buffer containing the data
    * @param flags see 'man send()'
    * @return {success, errno}
    */
    std::pair<bool, int> send_pkt(const pkt_buffer& buffer, const int flags = MSG_NOSIGNAL) const {
        size_t offset = 0;
        int result = 0;
        
        while (result >= 0 && offset < buffer.msg_length()) {
            result = send(this->m_skt, buffer.data() + offset, buffer.msg_length() - offset, flags);
            offset += result;
        } 
        
        return this->check_error(result);
    }

    /**
    * @brief receives maximal buffer.capacity-Bytes and stores them into the buffer
    *
    * @param buffer reference to the destination buffer
    * @param flags see 'man recv'
    * @return {success, errno}
    */
    std::pair<bool, int> recv_pkt(pkt_buffer& buffer, const int flags = 0) const {
        int result = recv(this->m_skt, buffer.data(), buffer.capacity(), flags);
        if (result < 1) {
            buffer.set_msg_length(0);
        } else {
            buffer.set_msg_length(result);
        }

        return this->check_error(result);
    }
};
}

#endif
