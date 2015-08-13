#include "tcpSocket.h"

class tcpServer : public tcpSocket {
public:
	~tcpServer();
	void acceptConnection();
	
	
	static void recvConnection();
	
	
	void startAccept();
	void startRecv();
	void bindSocket();
};
