

server: server.cpp
	g++ -o server server.cpp -std=c++11 -I../ -L../lib -lnetwork_static -lpthread -lrt

tcp_server: tcp_server.cpp
	g++ -o tcp_server tcp_server.cpp ip_addr.cpp.o -std=c++11 -I../ -L../lib -lpthread -lrt

tcp_server_debug: tcp_server.cpp
	g++ -g -o tcp_server tcp_server.cpp ip_addr.cpp.o -std=c++11 -I../ -L../lib -lpthread -lrt



client: client.cpp
	g++ -o client client.cpp -std=c++11 -I../ -L../lib -lnetwork_static -lpthread -lrt


ddos: ddos.cpp
	g++ -o ddos ddos.cpp -std=c++11 -I../ -L../lib -lnetwork_static -lpthread -lrt

