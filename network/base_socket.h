#ifndef base_socket_def
#define base_socket_def

#include "unistd.h"
#include "ip_addr.h"

namespace network {
    /**
    * @brief base class for general socket (UDP or TCP)
    * 
    */
    template<typename IP_ADDR_TYPE>
    class base_socket {
    public:
        base_socket() {
            m_skt = 0;
        }
        base_socket(base_socket& conn) = delete;
        base_socket(base_socket&& conn) {
            m_skt = conn.m_skt;
            conn.m_skt = 0;
            
            m_addr = conn.m_addr;
        }
        base_socket& operator=(base_socket&& conn) {            
            if (!is_closed()) {
                close_socket();
            }
            
            m_skt = conn.m_skt;
            conn.m_skt = 0;
            
            m_addr = conn.m_addr;
            
            return *this;
        }
        
        /**
        * @brief closes the socket
        * 
        */
        ~base_socket() {
            if (!is_closed()) {
                close_socket();
            }
        }
        
        /**
        * @brief address of the socket
        * 
        * @return IP_ADDR_TYPE&
        */
        IP_ADDR_TYPE& get_addr() {
            return m_addr;
        }
        
        /**
         * @brief IP-address of the socket
         * 
         * @return const IP_ADDR_TYPE&
         */
        const IP_ADDR_TYPE& get_addr() const {
            return m_addr;
        }
        
        /**
         * @brief sets timeout for blocking calls (recv, send)
         * 
         * @param sec timeout in seconds
         */
        void set_timeout(const float sec) const {
            struct timeval tv;
            tv.tv_sec = (int)sec;
            tv.tv_usec = (1000000.0 * sec);
            tv.tv_usec = tv.tv_usec % 1000000;
            setsockopt(m_skt, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&tv, sizeof(struct timeval));
            setsockopt(m_skt, SOL_SOCKET, SO_SNDTIMEO, (struct timeval*)&tv, sizeof(struct timeval));
        }
        
        /**
        * @brief sets new socket
        * 
        * @param skt: any socket (accordingly to the derived-class)
        */
        void set_socket(const int skt) {
            m_skt = skt;
        }
        
        /**
        * @brief 
        * - tries to close the socket
        * - see man 'close' for more information
        * 
        * @return bool: success
        */
        bool close_socket() {
            int tmp = m_skt;
            m_skt = 0;

            return close(tmp) == 0;
        }
                
        /**
        * @brief current close-state
        */
        bool is_closed() const {
            return m_skt == 0;
        }
        
        /**
        * @brief see man shutdown()
        */
        void shut_RD() const {
            shutdown(m_skt, SHUT_RD);
        }
        
        /**
        * @brief see man shutdown()
        */
        void shut_WR() const {
            shutdown(m_skt, SHUT_WR);
        }
                        
        /**
        * @brief see man shutdown()
        */
        void shut_RDWR() const {
            shutdown(m_skt, SHUT_RDWR);
        }
    protected:
        // socket-descriptor
        int m_skt;
        // address of the socket
        IP_ADDR_TYPE m_addr;
    };
}

#endif

