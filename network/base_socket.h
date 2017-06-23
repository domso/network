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
            skt_ = 0;
        }
        base_socket(base_socket& conn) = delete;
        //______________________________________________________________________________________________________
        //
        // Description:
        // - closes the socket
        //______________________________________________________________________________________________________
        ~base_socket() {
            closeSocket();
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - address of the socket
        //______________________________________________________________________________________________________
        IP_ADDR_TYPE& getAddr() {
            return addr_;
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - sets timeout for blocking calls (recv,send)
        // Parameter:
        // - sec: maximal number of seconds any blocking-call will wait
        //______________________________________________________________________________________________________
        void setTimeout(const float sec) const {
            struct timeval tv;
            tv.tv_sec = (int)sec;
            tv.tv_usec = (1000000.0 * sec);
            tv.tv_usec = tv.tv_usec % 1000000;
            setsockopt(skt_, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&tv, sizeof(struct timeval));
            setsockopt(skt_, SOL_SOCKET, SO_SNDTIMEO, (struct timeval*)&tv, sizeof(struct timeval));
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - sets new socket
        // Parameter:
        // - skt: any socket (accordingly to the child-class)
        //______________________________________________________________________________________________________
        void setSocket(int skt) {
            skt_ = skt;
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
        bool closeSocket() {
            int tmp = skt_;
            skt_ = 0;

            return close(tmp) == 0;
        }
        //______________________________________________________________________________________________________
        //
        // Return:
        // - current close-state
        //______________________________________________________________________________________________________
        bool isClosed() const {
            return skt_ == 0;
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - see man shutdown()
        //______________________________________________________________________________________________________
        void shutRD() const {
            shutdown(skt_, SHUT_RD);
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - see man shutdown()
        //______________________________________________________________________________________________________
        void shutWR() const {
            shutdown(skt_, SHUT_WR);
        }
        //______________________________________________________________________________________________________
        //
        // Description:
        // - see man shutdown()
        //______________________________________________________________________________________________________
        void shutRDWR() const {
            shutdown(skt_, SHUT_RDWR);
        }
    protected:
        // socket-descriptor
        int skt_;
        // address of the socket
        IP_ADDR_TYPE addr_;
    };
}

#endif

