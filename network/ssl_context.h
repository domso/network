#pragma once

#include <string>
#include <optional>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "ssl_connection.h"

namespace network {
template<typename IP_ADDR_TYPE>
class ssl_context {
public:
    ssl_context() : m_context(nullptr) {
        SSL_load_error_strings();
        OpenSSL_add_ssl_algorithms();        
    }
    ssl_context(const ssl_context& copy) = delete;
    ssl_context(ssl_context&& move) = delete;

    ~ssl_context() {
        if (m_context != nullptr) {
            SSL_CTX_free(m_context);
        }
        
        EVP_cleanup();
    }

    bool init(std::string cert_file, std::string key_file) {
        const SSL_METHOD* method = SSLv23_server_method();
        m_context = SSL_CTX_new(method);   
        
        if (m_context != nullptr) {
            SSL_CTX_set_ecdh_auto(ctx, 1);
            
            return SSL_CTX_use_certificate_file(m_context, cert_file.c_str(), SSL_FILETYPE_PEM) == 1 &&
                   SSL_CTX_use_PrivateKey_file(m_context, key_file.c_str(), SSL_FILETYPE_PEM) == 1;
        }
        
        return false;
    }

    std::optional<ssl_connection<IP_ADDR_TYPE>> accept(tcp_connection<IP_ADDR_TYPE>&& connection) {
        SSL* ssl = SSL_new(m_context);
        SSL_set_fd(ssl, connection.get_socket());             
        if (SSL_accept(ssl) <= 0) { 
            return std::nullopt;           
        }
        
        return std::move(ssl_connection<IP_ADDR_TYPE>(std::move(connection), ssl));        
    }    
    
    ssl_connection<IP_ADDR_TYPE> convert(tcp_connection<IP_ADDR_TYPE>&& connection) {
        SSL* ssl = SSL_new(m_context);
        SSL_set_fd(ssl, connection.get_socket());             
        
        return std::move(ssl_connection<IP_ADDR_TYPE>(std::move(connection), ssl));           
    }
    
    SSL_CTX* context() {
        return m_context;
    }
private:
    SSL_CTX* m_context;
};

}
