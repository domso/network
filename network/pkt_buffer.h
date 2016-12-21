#ifndef PKT_BUFFER_H
#define PKT_BUFFER_H

#include <vector>
#include "network/ip_addr.h"

namespace network {
  
    template<typename IP_ADDR_TYPE> 
    struct pkt_buffer {
        pkt_buffer(int buffer_size) : msg_buffer(buffer_size) {
            
        }
        IP_ADDR_TYPE addr;
        std::vector<char> msg_buffer;
        int msgLen;
    };

}
#endif
