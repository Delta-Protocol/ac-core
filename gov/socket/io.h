#ifndef USGOV_7f241fe1275bd94e413870e5de9bcb3246978771fa902243ebc9ab5eb5e1ddb0
#define USGOV_7f241fe1275bd94e413870e5de9bcb3246978771fa902243ebc9ab5eb5e1ddb0

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <csignal>
#include <cassert>
#include <memory>
#include <mutex>
#include <gov/net.h>
#include "datagram.h"

namespace usgov {
namespace socket {
	using namespace std;
	struct io {
		static datagram* send_recv(int sock, const string& msg);
		static datagram* send_recv(int sock, datagram* d);
		static bool send(int sock, datagram*);
		static bool send(int sock, char);
		static bool send(int sock, const datagram& d) { return d.send(sock); }
		long ping(int sock);
	};

}
}

#endif

