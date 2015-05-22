#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include "unistd.h"
#include "thread.h"

class tcpSocket {

public:
	int socketID;
	bool isServer;
	struct addrinfo host_info;
	struct addrinfo *host_info_list;
public:
	tcpSocket(std::string IP = "");	
	~tcpSocket();
	void setUpHostInfo();
	void closeSockets();
};


class tcpServer : public tcpSocket {
	threadController<int> acceptConnectionController;
	
	
	threadController<int> recvController;
public:
	~tcpServer();
	void acceptConnection(threadController< int >* controller);
	
	
	static void recvConnection(threadController<int>* controller);
	
	
	void startAccept();
	void startRecv();
	void bindSocket();
};





class tcpClient : tcpSocket {
	void connectSocket();
};