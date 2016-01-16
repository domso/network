#include <iostream>

#include <vector>
#include "unistd.h"

#include "network/udp_receiver.h"
#include "network/ip_pkg.h"

#include "network/rw_mutex.h"

#include "network/rw_container.h"

#define NUM_PKG 1000000

void callback(network::ip_pkg& pkg, const network::udp_socket& socket, const void* addPtr) {
    std::cout << std::string(&pkg.getData()[0]) << std::endl;
    if (pkg.compare("ADDR")) {
        std::string IP;
        std::string PORT;
        int mode = 0;
        for (int i = 5; i < pkg.length(); i++) {

            if (pkg.getData()[i] == ':') {
                mode = 1;
            } else if (pkg.getData()[i] == '|') {
                mode = 0;
                network::ipv4_addr client;
                client.init(IP,std::stoi(PORT));
//                 std::cout << "send to " << IP << ":" << std::stoi(PORT) << std::endl;
                socket.send(client, "HI-");   
                IP ="";
                PORT = "";
            } else {
                if (mode == 0) {
                    IP += pkg.getData()[i];
                }
                if (mode == 1) {
                    PORT += pkg.getData()[i];
                }
            }

        }
    }
}

void client() {
    network::udp_socket sock;
    network::udp_receiver receiver;

    if (!sock.init(5002, AF_INET)) {
        std::cout << "error while init socket!" << std::endl;

    }

    receiver.init(sock, callback, nullptr);

    network::ipv4_addr partner;
    partner.init("85.25.192.29", 5000);
    sock.send(partner, "CON");	    
    std::cin.get();

    sock.send(partner, "FIN");
    receiver.stop();
}

int main(int argc, char** argv) {
    client();
}


