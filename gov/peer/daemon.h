#ifndef USGOV_bf993116c1d89300316c0dd4fa2ef165d5b9128da89d33c44cfe50e95622fce8
#define USGOV_bf993116c1d89300316c0dd4fa2ef165d5b9128da89d33c44cfe50e95622fce8

//#include <us/gov/socket/daemon.h>
#include <us/gov/auth/daemon.h>
#include <vector>
#include <thread>
//#include <condition_variable>
#include <iostream>
#include <random>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <cassert>
#include "peer_t.h"

namespace us { namespace gov {
namespace peer {
	using namespace std;

	using socket::datagram;

//	struct daemon: socket::daemon {
	struct daemon: auth::daemon {
//		typedef socket::daemon b;
		typedef auth::daemon b;
		daemon();
		daemon(uint16_t port, uint16_t edges);
		virtual ~daemon();
//        	virtual socket::client* create_client(int sock) override;

		virtual void daemon_timer() override;


		struct peers_t:vector<peer_t*> {
			size_t asize() const {
					size_t n=0;
					for (auto i:*this) if (i!=0) ++n;
					return n;
			}
			void dump(ostream&os) const {
				for (auto i:*this) {
					i->dump(os);
					os << endl;
				}
			}
		};

		peers_t connected_peers() const;
		//bool is_connecte(const pub_t&)

		void dump(ostream& os) const;

		void send(int num, peer_t* exclude, datagram* d);
		void check_latency(const peers_t&);

		virtual string get_random_peer(const unordered_set<string>& exclude) const=0; // { return ""; }

		vector<peer_t*> in_service() const;
		vector<peer_t*> in_service(const peers_t& a) const;
		void purge_slow(peers_t&a);

		void purge_excess(peers_t& a);
		void purge_peers(peers_t& a);
		void add_peers(peers_t& a);

		peers_t active() const;
		peers_t adjust_peer_number();

		void set_tor() { set_mode(0); }
		void set_ip4() { set_mode(1); }

		peers_t::iterator oldest(peers_t& v) const;
		void set_mode(int mode);

	private:
		uint16_t edges;
	};

}

}}

#endif

