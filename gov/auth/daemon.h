#ifndef USGOV_c009bd5d681264adabb9cdbf5de437c3b84e5676690f277d47a56ef5388b9837
#define USGOV_c009bd5d681264adabb9cdbf5de437c3b84e5676690f277d47a56ef5388b9837

#include <us/gov/id/daemon.h>

namespace us { namespace gov {
namespace auth {

using namespace std;


typedef id::daemon daemon;

/*
struct daemon:id::daemon {
	typedef id::daemon b;

	daemon();
    daemon(uint16_t port, uint16_t edges);
	virtual ~daemon();

//	virtual socket::client* create_client(int sock) override=0;
	void dump(ostream& os) const;

//	virtual bool process_work(socket::peer_t *p, datagram*d) override;

};
*/

}
}}

#endif

