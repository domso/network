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
	tcpSocket();	
	~tcpSocket();
	void setUpHostInfo();
	void closeSockets();
};