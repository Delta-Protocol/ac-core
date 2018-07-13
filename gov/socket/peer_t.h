#ifndef USGOV_2d8af251450aa79ce1d82cd9d9330072930013905de9945ca29228083ef3eee4
#define USGOV_2d8af251450aa79ce1d82cd9d9330072930013905de9945ca29228083ef3eee4

#include "client.h"

namespace us { namespace gov {
namespace socket {
	using namespace std;
	struct daemon;

	struct peer_t: client {
		typedef client b;

		peer_t();
		peer_t(int sock);
		virtual ~peer_t();
		virtual bool process_work(datagram* d) { return false; }
//		static pair<string,datagram*> send_recv(const string&addr, uint16_t port, datagram*d);
        pair<string,datagram*> send_recv(datagram* d); 

		virtual void ready() override;
		void dump(ostream& os) const;
		virtual void on_connect() override;
		
		daemon* parent{0};
	};
}
}}

#endif

