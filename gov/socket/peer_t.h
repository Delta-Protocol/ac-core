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
        void process_pong();
	bool is_slow() const; //take a lock before call

        bool ping();

	bool process_work(datagram* d);
	virtual void ready() override;
	void dump(ostream& os) const {
	}
	virtual void dump_all(ostream& os) const override {
		dump(os);
		b::dump_all(os);
	}
	virtual void on_connect() override;
	daemon* parent{0};
	// chrono::microseconds latency;
	};
}
}}

#endif

