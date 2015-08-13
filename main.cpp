#include <iostream>
#include "thread.h"
#include <vector>
#include "unistd.h"
#include "server.h"
#include "msgLog.h"

#include "semaphore.h"

#include "simpleList.h"

#include "ip_addr.h"
#include "udp_socket.h"
#include "udp_receiver.h"
#include "ip_pkg.h"

void foo(threadController<int> * controller) {
	int t = 0;
	while (controller->isRunning()) {
		t = controller->recv_input();
		if (t != 0) {
			std::cout << (t) << std::endl;
		}
	}
	std::cout << "finish" << std::endl;
	return;
}


class test : public simpleList{
public:
  
  int x;

};

void callBack_server(network::ip_addr& addr,std::vector<char>& buffer,int recvBytes,network::udp_socket& socket){
    //std::string tmp(buffer.data());
   // std::cout << "Data: '" << tmp << "' (" << recvBytes << " Bytes) from " << inet_ntoa(addr.getSockaddr_in().sin_addr) << ":" << ntohs(addr.getSockaddr_in().sin_port) << std::endl;
    
    network::ip_pkg pkg(buffer,recvBytes,addr);
    
    std::cout << pkg.toString() << std::endl;
    
    socket.send(addr,buffer,recvBytes);

}
void callBack_client(network::ip_addr& addr,std::vector<char>& buffer,int recvBytes,network::udp_socket& socket){
    std::string tmp(buffer.data());
    std::cout << "Data: '" << tmp << "' (" << recvBytes << " Bytes) from " << inet_ntoa(addr.getSockaddr_in().sin_addr) << ":" << ntohs(addr.getSockaddr_in().sin_port) << std::endl;
}

void server(){
   network::udp_socket sock;
  
  if(!sock.init(5000)){
    std::cout << "error while init socket!" << std::endl;
    
  }
  network::udp_receiver receiver(sock);
  
  receiver.init(1000,&callBack_server );
 
  std::cin.get();
}

void client(){
  network::udp_socket sock;
  network::udp_receiver receiver(sock);
  
  receiver.init(1000,&callBack_client);
  
  if(!sock.init(5002)){
    std::cout << "error while init socket!" << std::endl;
    
  }
  
  network::ip_addr partner;
  partner.init("127.0.0.1",5000);
  
  std::vector<char> buffer(500,'\0');
  
  buffer[0] = 'H';
  buffer[1] = 'A';
  buffer[2] = 'L';
  buffer[3] = 'L';
  buffer[4] = 'O';

  
  sock.send(partner,buffer,5);
  
//  std::cin.get();
}

int main(int argc, char **argv) {

  //server();
  client();

  
  
}


