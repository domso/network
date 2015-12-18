#include <iostream>
#include <set>

#include "network/udp_receiver.h"
#include "network/rw_mutex.h"
#include "network/rw_container.h"

rw_container<std::set<std::string>>* rw_callback_set;
void callback(network::ip_pkg& pkg, const network::udp_socket& socket, const void* addPtr) {
    bool CON2GET = false;
    if (pkg.compare("CON")) {
        network::ipv4_addr& ipv4 = (static_cast<network::ipv4_addr&>(pkg.getAddr()));
        std::string spkg = ipv4.getIP() + ":" + std::to_string(ipv4.getPort());
        rw_callback_set->write().lock();
        rw_callback_set->data().insert(spkg);
        rw_callback_set->write().unlock();
        CON2GET = true;
        socket.send(pkg.getAddr(), "CON-OK");
    }
    if (pkg.compare("GET") || CON2GET) {
        std::string output;
        rw_callback_set->read().lock();
        socket.send(pkg.getAddr(), "GET-START:" + std::to_string(rw_callback_set->data().size()) + "|");
        for (std::string s : rw_callback_set->data()) {
            output += s + "|";
            if (output.length() > 1000) {
                socket.send(pkg.getAddr(), output);
                output = "";
            }
        }

        rw_callback_set->read().unlock();
        if (output != "") {
            socket.send(pkg.getAddr(), output);
        }
        return;
    }
    if (pkg.compare("FIN")) {
        bool ok = false;
        network::ipv4_addr& ipv4 = (static_cast<network::ipv4_addr&>(pkg.getAddr()));
        std::string spkg = ipv4.getIP() + ":" + std::to_string(ipv4.getPort());
        rw_callback_set->write().lock();
        if (rw_callback_set->data().erase(spkg) > 0) {
            ok = true;
        }
        rw_callback_set->write().unlock();
        if (ok) {
            socket.send(pkg.getAddr(), "FIN-OK");
        } else {
            socket.send(pkg.getAddr(), "FIN-FAIL");
        }
    }
}

void server() {
    network::udp_socket sock;
    network::udp_receiver receiver;
    rw_container<std::set<std::string>> rw_set;
    rw_callback_set = &rw_set;

    if (!sock.init(5000, AF_INET)) {
        std::cout << "error while init socket!" << std::endl;
    }
    receiver.init(sock, &callback, nullptr);




    std::cin.get();
    receiver.stop();
}

int main(int argc, char** argv) {
    server();
}


