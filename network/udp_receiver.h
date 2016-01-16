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
#define NETWORK_UDP_RECEIVER_INIT_PARAM_SEC2WAIT 1
#define NETWORK_UDP_RECEIVER_INIT_PARAM_MINTHREAD 2
#define NETWORK_UDP_RECEIVER_INIT_PARAM_MAXTHREAD 2



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
                //int maxThread; // maximum number of workThread-instances(=threads)               
                //int minThread; // minimum number of workThread-instances(=threads)     
                //void* addPtr // additional pointer, which will be passed to work_callbackFunction (default nullptr)


            struct udp_receiver_init_param {
                // default constructor
                // sets all default parameters
                udp_receiver_init_param();
                // size of the recv-buffer
                int bufferSize;
                // timeout for blocking calls
                int sec2wait;
                // maximum number of workThread-instances(=threads)
                int maxThread;
                // minimum number of workThread-instances(=threads)
                int minThread;
                // additional pointer, which will be passed to work_callbackFunction
                void* addPtr;

            };
            // parameters:
            // -> socket: the udp_socket for receiving
            // -> work_callbackFunction: function which will be executed with incoming packages
            // -> udp_receiver_init_param: optional parameters
            void init(const network::udp_socket socket, void (*const work_callbackFunction)(ip_pkg&, const udp_socket&, const void* addPtr),const udp_receiver::udp_receiver_init_param* const parameters) const;
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
                void init(const network::udp_socket skt, void (*const work_cbFunction)(ip_pkg&, const udp_socket&, const void* addPtr),const udp_receiver::udp_receiver_init_param* const parameters);
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // receiver-thread
                static void recvThread(network::udp_receiver::udp_receiver_data* receiver);
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // controller-thread:
                static void contThread(network::udp_receiver::udp_receiver_data* receiver, int pollIntervall);
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // semaphore for smartPointer
                sem_t semaphore;
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
                // callback for incoming data (executed by workThread)
                void (*work_callbackFunction)(ip_pkg&, const udp_socket&, const void*);
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // package-queue: recvThread --> workThread
                shared_queue<ip_pkg> queue;
                //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
                // size of recv-buffer
                int bufferSize;
                // timeout for blocking calls
                int sec2wait;
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

