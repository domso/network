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

#include "run_lock.h"

namespace network {
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // smartPointer-callback-receiver for udp_socket
    class udp_receiver {
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        public:
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // default constructor
            udp_receiver();
            // copy constructor
            udp_receiver(const udp_receiver& that);
            // default destructor
            ~udp_receiver();
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // assignment for smartPointer
            void operator =(const udp_receiver that);
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // parameters:
            // -> socket: the udp_socket for receiving
            // -> bufferSize: size of the recv-buffer
            // -> recv_callbackFunction: function which will be executed every time data arrives the recv-thread (nullptr for disable) if the return-value is NOT zero the data will be ignored
            // -> work_callbackFunction: function which will be executed with a ip_pkg arrives a working-Thread (not nullptr!) put all package-processing here
            void init(const network::udp_socket socket, const int bufferSize, int (*const recv_callbackFunction)(const ip_addr&, std::vector<char>&, const int, const udp_socket), void (*const work_callbackFunction)(ip_pkg, const udp_socket), const int minThread , const int maxThread) const;
            // stops all threads
            // -> udp_receiver_data will NOT be freed until stop() is called!
            void stop();
			//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        private:
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // data-struct for smartPointer
            struct udp_receiver_data {
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                udp_receiver_data();
                ~udp_receiver_data();
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // set all parameters
                void init(const network::udp_socket skt, const int bufferSize, int (*const recv_cbFunction)(const ip_addr&, std::vector<char>&, const int, const udp_socket), void (*const work_cbFunction)(const ip_pkg, const udp_socket), const int minThread, const int maxThread);
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // receiver-thread
                // -> socket.recv()
                // -> if recv_callbackFunction() is not set or returns 0 create ip_pkg
                // -> store ip_pkg in queue
                // -> holds an udp_receiver-obj; --> udp_receiver_data guaranteed exist
                static void recvThread(network::udp_receiver::udp_receiver_data* receiver, int pollIntervall);
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // worker-thread:
                // -> pops package from queue
                // -> executes work_callbackFunction() for every package
                // -> notifies contThread to start a workThread-instance (dynamic-thread)
                // -> close if dynamic and wait-time reached (sec2close)
                // -> holds an udp_receiver-obj; --> udp_receiver_data guaranteed exist
                static void workThread(network::udp_receiver::udp_receiver_data* receiver, bool isMainThread);
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // controller-thread:
                // -> starts all initial-threads (minThread)
                // -> starts new dynamic-threads (maxThread - minThread)
                // -> waits until all threads are closed (join)
                // -> holds an udp_receiver-obj; --> udp_receiver_data guaranteed exist
                static void contThread(network::udp_receiver::udp_receiver_data* receiver, int pollIntervall);
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // semaphore for smartPointer
                sem_t semaphore;
                // semaphores for dynamic thread pool
                sem_t dynamicThreadNum;
                sem_t numThread;
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // threadpool-state (running|stop)
                run_lock threadState;
                // UDP-socket (=smartPointer!)
                udp_socket socket;
                // thread-handle for contThread();
                std::thread cont_thread;
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // buffer for socket.recv()
                std::vector<char> buffer;
                // ip_addr for socket.recv()
                ip_addr recv_addr;
                // callback for incoming data (executed by recvThread)
                int (*recv_callbackFunction)(const ip_addr&, std::vector<char>&, const int, const udp_socket);
                // callback for incoming data (executed by workThread)
                void (*work_callbackFunction)(ip_pkg, const udp_socket);
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // package-queue: recvThread --> workThread
                shared_queue<ip_pkg> queue;
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // timeout for blocking calls
                int sec2wait;
                // wait-time for dynamic thread to close
                int sec2close;
                // time a package needs to spend in the queue for starting a new dynamic thread
                int microsec2start;
                // maximum number of workThread-instances(=threads)
                int maxThread;
                // minimum number of workThread-instances(=threads)
                int minThread;

            };
            // private constructor to create a new smartPointer-Object from the internal ptr
            udp_receiver(network::udp_receiver::udp_receiver_data* dataPTR);
            //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
            // ptr to the internal data
            udp_receiver_data* dataPTR_;

    };
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
}




#endif
