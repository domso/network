#ifndef base_socket_def
#define base_socket_def

#include "unistd.h"
#include "ip_addr.h"

namespace network {
    //______________________________________________________________________________________________________
    //
    // base class for general socket (UDP or TCP)
    //______________________________________________________________________________________________________
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
        //______________________________________________________________________________________________________
        //
        // Description:
        // - closes the socket
        //______________________________________________________________________________________________________
        ~base_socket() {
            if (!is_closed()) {
                close_socket();
            }
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - address of the socket
        //______________________________________________________________________________________________________
        IP_ADDR_TYPE& get_addr() {
            return m_addr;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - const address of the socket
        //______________________________________________________________________________________________________
        const IP_ADDR_TYPE& get_addr() const {
            return m_addr;
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - sets timeout for blocking calls (recv,send)
        // Parameter:
        // - sec: maximal number of seconds any blocking-call will wait
        //______________________________________________________________________________________________________
        void set_timeout(const float sec) const {
            struct timeval tv;
            tv.tv_sec = (int)sec;
            tv.tv_usec = (1000000.0 * sec);
            tv.tv_usec = tv.tv_usec % 1000000;
            setsockopt(m_skt, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&tv, sizeof(struct timeval));
            setsockopt(m_skt, SOL_SOCKET, SO_SNDTIMEO, (struct timeval*)&tv, sizeof(struct timeval));
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - sets new socket
        // Parameter:
        // - skt: any socket (accordingly to the child-class)
        //______________________________________________________________________________________________________
        void set_socket(const int skt) {
            m_skt = skt;
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - gets current socket
        //______________________________________________________________________________________________________
        int get_socket() {
            return m_skt;
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - tries to close the socket
        // - see man 'close' for more information
        // Return:
        // - true: the socket could closed succesfully or was already closed
        // - false: on any error
        //______________________________________________________________________________________________________
        bool close_socket() {
            int tmp = m_skt;
            m_skt = 0;

            return close(tmp) == 0;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - current close-state
        //______________________________________________________________________________________________________
        bool is_closed() const {
            return m_skt == 0;
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - see man shutdown()
        //______________________________________________________________________________________________________
        void shut_RD() const {
            shutdown(m_skt, SHUT_RD);
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - see man shutdown()
        //______________________________________________________________________________________________________
        void shut_WR() const {
            shutdown(m_skt, SHUT_WR);
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - see man shutdown()
        //______________________________________________________________________________________________________
        void shut_RDWR() const {
            shutdown(m_skt, SHUT_RDWR);
        }
    protected:  
        //______________________________________________________________________________________________________
        //
        // Description:
        // - if the given argument is -1, returns errno, otherwise returns zero
        //______________________________________________________________________________________________________
        std::pair<bool, int> check_error(int result) const {
            if (result == -1) {
                return std::make_pair((errno == EAGAIN || errno == EWOULDBLOCK), errno);
            } else if (result == 0) {
                return std::make_pair(false, 0);
            } else {
                return std::make_pair(true, result);
            }
        }
        
        // socket-descriptor
        int m_skt;
        // address of the socket
        IP_ADDR_TYPE m_addr;
    };
}

#endif

