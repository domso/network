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
        virtual uint16_t getPort() const = 0;
        //______________________________________________________________________________________________________
        //
        // Return:
        // - IP in the standardized textform
        //______________________________________________________________________________________________________
        virtual const std::string getIP() const = 0;
        //______________________________________________________________________________________________________
        //
        // Return:
        // - socket family AF_INET/AF_INET6
        //______________________________________________________________________________________________________
        virtual int getFamily() const = 0;
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
            memset((char*) &network_addr_v4_, 0, sizeof(network_addr_v4_));

            network_addr_v4_.sin_family = AF_INET;
            network_addr_v4_.sin_port = htons(port);
            network_addr_v4_.sin_addr.s_addr = htonl(INADDR_ANY);

            if (ip != "" && inet_pton(AF_INET, ip.c_str() , &network_addr_v4_.sin_addr) == 1) {
                return true;
            }

            return false;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - port-number
        //______________________________________________________________________________________________________
        uint16_t getPort() const {
            return htons(network_addr_v4_.sin_port);
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - IP in the standardized textform
        //______________________________________________________________________________________________________
        const std::string getIP() const {
             char buffer_v4[INET_ADDRSTRLEN];
             inet_ntop(AF_INET, &network_addr_v4_.sin_addr, buffer_v4, INET_ADDRSTRLEN);
             std::string tmp_v4(buffer_v4);
             return tmp_v4;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - the internal data
        //______________________________________________________________________________________________________
        const sockaddr_in* getSockaddr_in() const {
            return &network_addr_v4_;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - socket family AF_INET/AF_INET6
        //______________________________________________________________________________________________________
        int getFamily() const {
            return AF_INET;
        }
    protected:
        // internal data
        sockaddr_in network_addr_v4_;
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
            memset((char*) &network_addr_v6_, 0, sizeof(network_addr_v6_));

            network_addr_v6_.sin6_family = AF_INET6;
            network_addr_v6_.sin6_port = htons(port);
            network_addr_v6_.sin6_addr = in6addr_any;

            if (ip != "" && inet_pton(AF_INET6, ip.c_str() , &network_addr_v6_.sin6_addr) == 1) {
                return true;
            }

            return false;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - port-number
        //______________________________________________________________________________________________________
        uint16_t getPort() const {
            return network_addr_v6_.sin6_port;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - IP in the standardized textform
        //______________________________________________________________________________________________________
        const std::string getIP() const {
            char buffer_v6[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, &network_addr_v6_.sin6_addr, buffer_v6, INET6_ADDRSTRLEN);
            std::string tmp_v6(buffer_v6);
            return tmp_v6;
        }
        //______________________________________________________________________________________________________
        //
        // returns the internal data
        //______________________________________________________________________________________________________
        const sockaddr_in6* getSockaddr_in() const {
            return &network_addr_v6_;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - socket family AF_INET/AF_INET6
        //______________________________________________________________________________________________________
        int getFamily() const {
            return AF_INET6;
        }
    protected:
        // internal data
        sockaddr_in6 network_addr_v6_;
    };
}

#endif

