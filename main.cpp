#include <iostream>
#include "thread.h"
#include <vector>
#include "unistd.h"
#include "server.h"
#include "msgLog.h"

using namespace std;
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



int main(int argc, char **argv) {

	
	/*
	int i =25;                                                 // new int(25);
	
	threadController<int> tmp(new std::thread(foo, &tmp));
	sleep(2);
	tmp.send_input(i);
	sleep(2);
	*/
	

	tcpServer server;
	server.bindSocket();
	std::cout << "start accept" << std::endl;
	server.startAccept();
	std::cout << "start recv" << std::endl;
	server.startRecv();
	std::cout << "wait" << std::endl;
	while (1) {;}

	/*

	
	std::cout <<  "oij" << std::endl;
	
	msgLog log;
	log.warning("nothing happend");
	log.error("bad things happend");
	log.msg("1 + 1 = 2; in case u forgot");
	//log.out();
	log.out();
	log.appendfile("test.txt");
	log.clear();
	//server.acceptSocket();
	*/
	return 0;
	
}
