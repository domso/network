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


#define NETWORK_UDP_RECEIVER_INIT_PARAM_BUFFERSIZE 1000
#define NETWORK_UDP_RECEIVER_INIT_PARAM_SEC2WAIT 10
#define NETWORK_UDP_RECEIVER_INIT_PARAM_SEC2CLOSE 100
#define NETWORK_UDP_RECEIVER_INIT_PARAM_MICROSEC2START 1000
#define NETWORK_UDP_RECEIVER_INIT_PARAM_MINTHREAD 2
#define NETWORK_UDP_RECEIVER_INIT_PARAM_MAXTHREAD 2
#define NETWORK_UDP_RECEIVER_INIT_PARAM_NUMPRIORITY 1



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
            // additional parameters for init()  
                //int bufferSize; // size of the recv-buffer   
                //int sec2wait; // timeout for blocking calls               
                //int sec2close; // wait-time for dynamic thread to close              
                //int microsec2start; // time a package needs to spend in the queue for starting a new dynamic thread               
                //int maxThread; // maximum number of workThread-instances(=threads)               
                //int minThread; // minimum number of workThread-instances(=threads)             
                //int numPriority; // number of package-queue priorities
                //void* addPtr // additional pointer, which will be passed to work_callbackFunction (default nullptr)


            struct udp_receiver_init_param {
                // default constructor
                // sets all default parameters
                udp_receiver_init_param();
                // size of the recv-buffer
                int bufferSize;
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
                // number of package-queue priorities
                int numPriority;
                // additional pointer, which will be passed to work_callbackFunction
                void* addPtr;

            };
            // parameters:
            // -> socket: the udp_socket for receiving
            // -> recv_callbackFunction: function which will be executed every time data arrives the recv-thread (nullptr for disable) if the function returns a negative number, the data will be ignored and deleted. if the return-value is zero, the package will be 'send' to work_callbackFunction() with highest priority. Higher return-values will reduce the priority (--> numPriority in udp_receiver::udp_receiver_init_param)
            // -> work_callbackFunction: function which will be executed with a ip_pkg arrives a working-Thread (not nullptr!) put all package-processing here
            // -> udp_receiver_init_param: optional parameters
            void init(const network::udp_socket socket, int (*const recv_callbackFunction)(const ip_addr&, std::vector<char>&, const int, const udp_socket), void (*const work_callbackFunction)(ip_pkg, const udp_socket, const void* addPtr),const udp_receiver::udp_receiver_init_param* const parameters) const;
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
                void init(const network::udp_socket skt, int (*const recv_cbFunction)(const ip_addr&, std::vector<char>&, const int, const udp_socket), void (*const work_cbFunction)(const ip_pkg, const udp_socket, const void* addPtr),const udp_receiver::udp_receiver_init_param* const parameters);
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
                void (*work_callbackFunction)(ip_pkg, const udp_socket, const void*);
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // package-queue: recvThread --> workThread
                shared_queue<ip_pkg> queue;
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // size of recv-buffer
                int bufferSize;
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
                // number of queue priorities
                int numPriority;
                // additional pointer, which will be passed to work_callbackFunction
                void* addPtr;

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
