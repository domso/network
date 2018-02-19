#ifndef ip_addr_h
#define ip_addr_h

#include <string>
#include <string.h>
#include <arpa/inet.h>

namespace network {
    //______________________________________________________________________________________________________
    //
    // parent wrapper-class any ip-address
    //______________________________________________________________________________________________________
    class ip_addr {
    public:
        ip_addr() {

        }
        ~ip_addr() {

        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - sets ip and port-number
        //______________________________________________________________________________________________________
        virtual bool init(const std::string ip, const uint16_t port) = 0;
        //______________________________________________________________________________________________________
        //
        // Return:
        // - port-number
        //______________________________________________________________________________________________________
        virtual uint16_t port() const = 0;
        //______________________________________________________________________________________________________
        //
        // Return:
        // - IP in the standardized textform
        //______________________________________________________________________________________________________
        virtual const std::string ip() const = 0;
        //______________________________________________________________________________________________________
        //
        // Return:
        // - socket family AF_INET/AF_INET6
        //______________________________________________________________________________________________________
        virtual int family() const = 0;
    };
    //______________________________________________________________________________________________________
    //
    // wrapper-class for ipv4-addresses (sockaddr_in)
    //______________________________________________________________________________________________________
    class ipv4_addr : public ip_addr {
    public:
        //______________________________________________________________________________________________________
        //
        // Description:
        // - sets ip and port-number
        //______________________________________________________________________________________________________
        bool init(const std::string ip, const uint16_t port) {
            memset((char*) &m_network_addr_v4, 0, sizeof(m_network_addr_v4));

            m_network_addr_v4.sin_family = AF_INET;
            m_network_addr_v4.sin_port = htons(port);
            m_network_addr_v4.sin_addr.s_addr = htonl(INADDR_ANY);

            if (ip != "" && inet_pton(AF_INET, ip.c_str() , &m_network_addr_v4.sin_addr) == 1) {
                return true;
            }

            return false;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - port-number
        //______________________________________________________________________________________________________
        uint16_t port() const {
            return htons(m_network_addr_v4.sin_port);
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - IP in the standardized textform
        //______________________________________________________________________________________________________
        const std::string ip() const {
             char buffer_v4[INET_ADDRSTRLEN];
             inet_ntop(AF_INET, &m_network_addr_v4.sin_addr, buffer_v4, INET_ADDRSTRLEN);
             std::string tmp_v4(buffer_v4);
             return tmp_v4;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - the internal data
        //______________________________________________________________________________________________________
        const sockaddr_in* internal_handle() const {
            return &m_network_addr_v4;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - socket family AF_INET/AF_INET6
        //______________________________________________________________________________________________________
        int family() const {
            return AF_INET;
        }
    protected:
        // internal data
        struct sockaddr_in m_network_addr_v4;
    };
    //______________________________________________________________________________________________________
    //
    // wrapper-class for ipv6-addresses (sockaddr_in6)
    //______________________________________________________________________________________________________
    class ipv6_addr : public ip_addr {
    public:
        //______________________________________________________________________________________________________
        //
        // Description:
        // - sets ip and port-number
        //______________________________________________________________________________________________________
        bool init(const std::string ip, const uint16_t port) {
            memset((char*) &m_network_addr_v6, 0, sizeof(m_network_addr_v6));

            m_network_addr_v6.sin6_family = AF_INET6;
            m_network_addr_v6.sin6_port = htons(port);
            m_network_addr_v6.sin6_addr = in6addr_any;

            if (ip != "" && inet_pton(AF_INET6, ip.c_str() , &m_network_addr_v6.sin6_addr) == 1) {
                return true;
            }

            return false;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - port-number
        //______________________________________________________________________________________________________
        uint16_t port() const {
            return m_network_addr_v6.sin6_port;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - IP in the standardized textform
        //______________________________________________________________________________________________________
        const std::string ip() const {
            char buffer_v6[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &m_network_addr_v6.sin6_addr, buffer_v6, INET6_ADDRSTRLEN);
            std::string tmp_v6(buffer_v6);
            return tmp_v6;
        }
        //______________________________________________________________________________________________________
        //
        // returns the internal data
        //______________________________________________________________________________________________________
        const sockaddr_in6* sockaddr_in() const {
            return &m_network_addr_v6;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - socket family AF_INET/AF_INET6
        //______________________________________________________________________________________________________
        int family() const {
            return AF_INET6;
        }
    protected:
        // internal data
        sockaddr_in6 m_network_addr_v6;
    };
}

#endif

