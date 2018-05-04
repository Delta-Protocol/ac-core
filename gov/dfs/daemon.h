#ifndef USGOV_1a04abafd244aa56917030342d01e6017e089433f96dd01ee598fed0d91162dd
#define USGOV_1a04abafd244aa56917030342d01e6017e089433f96dd01ee598fed0d91162dd

#include <gov/net.h>
#include <gov/auth.h>
#include <iostream>
#include <vector>

namespace usgov {
namespace dfs { //distributed file system
	using namespace std;
	typedef auth::peer_t peer_t;
	using socket::datagram;

	struct daemon:auth::daemon {
		typedef auth::daemon b;
		using b::keys; 

		daemon(const keys& k):b(k) {}
		daemon(const keys& k, uint16_t port, uint16_t edges):b(k, port,edges) {}
		virtual ~daemon() {}

		virtual bool process_work(socket::peer_t *c, datagram*d) override;
		void receive_file(peer_t* c, datagram* d);

		void send_file(peer_t* c, datagram* request);
		void save(const string& hash, const vector<uint8_t>& data, int propagate);  //-1 nopes, 0=all peers; n num of peers

		string load(const string& hash);

		static string resolve_filename(const string& filename);

	};

}

}
#endif

