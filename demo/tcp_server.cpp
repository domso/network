#include <iostream>
#include <vector>
#include <string>
#include "network/tcp_socket.h"


void server() {
	
	network::tcp_socket<network::ipv4_addr> socket;

	network::ipv4_addr addr;

	addr.init("", 80);

	if(!socket.acceptOn(addr, 10)) {
		std::cout << "Could not create tcp-socket" << std::endl;
	}


	while (true) {
		network::tcp_connection<network::ipv4_addr>* connection = socket.acceptConnection();

		network::ip_addr* testPtr;
		

		if (connection != nullptr) {
			std::cout << "new connection!" << std::endl;

			std::string message = "Hi!";
			std::vector<char> buffer(message.begin(), message.end());

			connection->sendData(buffer, buffer.size());

			delete connection;
		} else {
			std::cout << "error with new connection! o0 " << std::endl;
			return;

		}
	}

	while (!socket.closeSocket()) {}

}



int main(int argc, char** argv) {
    server();
}


