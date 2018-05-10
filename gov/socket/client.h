#ifndef USGOV_dfd7f74406ecd7bf1a137eafe457ab52dcb9c50fe177017cc67b73de69834ecb
#define USGOV_dfd7f74406ecd7bf1a137eafe457ab52dcb9c50fe177017cc67b73de69834ecb

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <csignal>
#include <cassert>
#include <memory>
#include <mutex>
#include <us/gov/net.h>
#include "datagram.h"
#include "io.h"
#include <iostream>

namespace us { namespace gov {
namespace socket {
	using namespace std;

	struct client {
		client();
		client(int sock);
		virtual ~client();
		virtual bool connect(const string& host, uint16_t port, bool block=false);
		virtual void disconnect();
		string address() const;
		virtual void ready() {}
		bool send(char d) const;
		bool send(datagram* d) const;
		bool send(const datagram& d) const;
		bool send(int service, const string& payload);

		void init_sockaddr (struct sockaddr_in *name, const char *hostname, uint16_t port);
		bool init_sock(const string& host, uint16_t port, bool block=false);

		virtual void on_connect() {}

		void dump(ostream& os) const;

		int sock;
		mutable string msg;
		string addr;
		mutable mutex mx;
	};

}
}}

#endif

