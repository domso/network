#include <iostream>
#include <memory.h>

#include "network/ip_addr.h"
#include "network/udp_socket.h"
#include "network/tcp_socket.h"
#include "network/tcp_connection.h"

struct msgProtocol {
    int status;
    char flags;
    char data[4];
};




void tcp_test() {

    network::tcp_socket<network::ipv4_addr> socket_in;

    if (!socket_in.accept_on(8005, 10)) {
        std::cout << "Could not create Socket!" << std::endl;
        return;
    }
    std::shared_ptr<network::tcp_connection<network::ipv4_addr>> new_connection;

    new_connection = socket_in.accept_connection();
    if (new_connection != nullptr) {
        std::cout << "new connection!" << std::endl;
        
        network::pkt_buffer buffer(1000);
        
        new_connection->recv_pkt(buffer);

        std::string msg = "HTTP-Version: HTTP/1.0 200 OK\nContent-Length: 0\nContent-Type: text/html\n<blank line>\n<body>";
      
        new_connection->send_data<char>(msg.data(), msg.size());
    }

}




void udp_test() {

    network::ipv4_addr addr;

    addr.init("127.0.0.1", 1234);

    network::udp_socket<network::ipv4_addr> socket;


    socket.init(346);

    

    msgProtocol msg;
    socket.recv_data<msgProtocol>(addr, &msg);
    if (msg.status == 0) {
        
    } else {
        
    }
        

    while (!socket.close_socket());


}



int main() {
    tcp_test();
    return 0;
}
