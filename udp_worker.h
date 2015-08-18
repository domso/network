#include <vector>
#include "ip_addr.h"
#include "ip_pkg.h"
#include "udp_socket.h"


namespace network{

	class udp_worker{

	public:
		
		udp_worker(ip_pkg pkg, udp_socket& sock);
		~udp_worker();
		
		
		
		
		void callBack_recv ( network::ip_addr& addr,std::vector<char>& buffer,int recvBytes,network::udp_socket& socket ) {
		}
	};

}


