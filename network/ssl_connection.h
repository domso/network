#pragma once

#include "tcp_connection.h"
#include <openssl/ssl.h>

namespace network {
/**
* @brief A wrapper for SSL-connections
*
*/
template<typename IP_ADDR_TYPE>
class ssl_connection {
public:
    ssl_connection(tcp_connection<IP_ADDR_TYPE>&& connection, SSL* sslHandle) : m_connection(std::move(connection)), m_ssl(sslHandle) {
        
    }
    
    ssl_connection(const ssl_connection& that) = delete;
    ssl_connection(ssl_connection&& conn) : m_connection(std::move(conn.m_connection)), m_ssl(std::move(conn.m_ssl)) {
        conn.m_ssl = nullptr;
    }
    ssl_connection& operator=(ssl_connection && conn) {
        m_connection(std::move(conn.m_connection));
        m_ssl = std::move(conn.m_ssl);
        conn.m_ssl = nullptr;
        
        return *this;
    }

    /**
    * @brief closes the connection
    *
    */
    ~ssl_connection() {
        if (m_ssl != nullptr) {
            SSL_free(m_ssl);
        }
    }

    /**
    * @brief sends numberOfData * sizeof(MSG_DATA_TYPE) Bytes
    *
    * @param MSG_DATA_TYPE type of the data
    * @param buffer buffer containing the data
    * @param numberOfData number of elements in the buffer
    * @return {success, number of send bytes}
    */
    template <typename MSG_DATA_TYPE>
    std::pair<bool, int> send_data(const MSG_DATA_TYPE* buffer, const int numberOfData) const {
        static_assert(std::is_trivially_copyable<MSG_DATA_TYPE>::value);
        
        int result = SSL_write(m_ssl, buffer, sizeof(MSG_DATA_TYPE) * numberOfData);
        return {result > 0, result};
    }

    /**
    * @brief receives maximal numberOfData * sizeof(MSG_DATA_TYPE) Bytes and stores them into the buffer
    *
    * @param MSG_DATA_TYPE type of the data
    * @param buffer destination buffer for new data
    * @param numberOfData size of buffer
    * @return {success, number of send bytes}
    */
    template <typename MSG_DATA_TYPE>
    std::pair<bool, int> recv_data(MSG_DATA_TYPE* buffer, const int numberOfData) const {
        static_assert(std::is_trivially_copyable<MSG_DATA_TYPE>::value);
        
        int result = SSL_read(m_ssl, buffer, sizeof(MSG_DATA_TYPE) * numberOfData);      
        return {result > 0, result};
    }

    /**
    * @brief sends buffer.msgLen()-Bytes
    *
    * @param buffer buffer containing the data
    * @return {success, number of send bytes}
    */
    std::pair<bool, int> send_pkt(const pkt_buffer& buffer) const {
        int result = SSL_write(m_ssl, buffer.data(), buffer.msg_length());
        return {result > 0, result};
    }

    /**
    * @brief receives maximal buffer.capacity-Bytes and stores them into the buffer
    *
    * @param buffer reference to the destination buffer
    * @return {success, number of send bytes}
    */
    std::pair<bool, int> recv_pkt(pkt_buffer& buffer) const {       
        int result = SSL_read(m_ssl, buffer.data(), buffer.capacity());   
        buffer.set_msg_length(result);
        return {result > 0, result};
    }

private:
    tcp_connection<IP_ADDR_TYPE> m_connection;
    SSL* m_ssl;
};
}



