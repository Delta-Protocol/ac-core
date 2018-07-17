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

        inline bool connected() const { return sock!=0; }

		string address() const;
		virtual void ready() {}

        pair<string,datagram*> send_recv(datagram* d); 
//		string send(char d) const;
		string send(datagram* d) const;
		string send(const datagram& d) const;
//		string send(int service, const string& payload);
        pair<string,datagram*> recv(); //caller owns the returning object

		void init_sockaddr (struct sockaddr_in *name, const char *hostname, uint16_t port);
		bool init_sock(const string& host, uint16_t port, bool block=false);

		virtual void on_connect() {}

		void dump(ostream& os) const;

        //void run_dialogue();

        //highest level function to receive a response

private:
        //lower level functions:
		//pair<string,datagram*> complete_datagram();
//		pair<string,datagram*> recv(int timeout_seconds);


//		datagram* curd{0};

public:
		int sock;
		mutable string msg;
		string addr;
		mutable mutex mx;
	};

}
}}

#endif

