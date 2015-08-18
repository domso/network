#ifndef udp_receiver_h
#define udp_receiver_h

#include <vector>
#include <mutex>
#include <condition_variable>
#include <thread>
#include "ip_addr.h"
#include "ip_pkg.h"
#include "udp_socket.h"
#include "shared_queue.h"

namespace network{
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
class udp_receiver{
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
public:
    udp_receiver ();
    udp_receiver ( const udp_receiver& that );
    ~udp_receiver();
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void init ( network::udp_socket socket, int bufferSize,int ( *recv_callbackFunction ) ( ip_addr&,std::vector<char>&,int,udp_socket ), void ( *work_callbackFunction ) ( ip_pkg, udp_socket ), int minThread , int maxThread);
    void stop();
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
private:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    struct udp_receiver_data {
        udp_receiver_data();
        ~udp_receiver_data();
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        void init ( network::udp_socket skt, int bufferSize,int ( *recv_cbFunction ) ( ip_addr&,std::vector<char>&,int,udp_socket ), void ( *work_cbFunction ) ( ip_pkg, udp_socket ), int minThread, int maxThread );
        static void recvThread ( network::udp_receiver::udp_receiver_data* receiver );
        static void workThread ( network::udp_receiver::udp_receiver_data* receiver, bool isMainThread);
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        sem_t semaphore;
        sem_t thread_sem;

        udp_socket socket;
        std::vector<char> buffer;
        int ( *recv_callbackFunction ) ( ip_addr&,std::vector<char>&,int,udp_socket );
        void ( *work_callbackFunction ) ( ip_pkg, udp_socket );
        ip_addr recv_addr;
        shared_queue<ip_pkg> queue;
        std::thread recv_thread;
		int sec2wait;
		int sec2close;
		int microsec2start;
    };
	udp_receiver( network::udp_receiver::udp_receiver_data* dataPTR );
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    udp_receiver_data* dataPTR_;

};
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}




#endif
