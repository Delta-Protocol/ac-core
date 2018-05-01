#ifndef USGOV_e71c29d5f8c07b8000435dfe6a5f1e49605b1b2041dad14a3ac898464ed5dd49
#define USGOV_e71c29d5f8c07b8000435dfe6a5f1e49605b1b2041dad14a3ac898464ed5dd49

#include <gov/net.h>
#include <gov/socket/peer_t.h>
#include <vector>
#include <thread>
#include <iostream>
#include <random>
#include <algorithm>
#include <sstream>
#include <chrono>
#include <cassert>
#include <array>
#include <unordered_set>
namespace usgov {
namespace peer {
	using namespace std;

	struct daemon;
	struct peer_t: socket::peer_t {
		typedef socket::peer_t b;

		using datagram=socket::datagram;

		enum stage_t {
			disconnected=0,
			connected,
			exceed_latency,
			service,
			disconnecting,
			num_stages
		};
		constexpr static array<const char*,num_stages> stagestr={"disconnected","connected","latency","service","disconnecting"};
		constexpr static array<const char*,2> modestr={"tor","ip4"};
		peer_t(int sock);
		virtual ~peer_t();
		datagram* complete_datagram();

		virtual bool connect(const string& host, uint16_t port, bool block=false) override;

		virtual void on_connect();

		virtual bool process_work(datagram* d);
		void process_pong();

		bool is_slow() const; //take a lock before call

		bool ping();
		void set_mode(int m) {
			lock_guard<mutex> lock(mx);
			mode=m;
		}

		virtual void disconnect() override;

		void dump(ostream& os) const;
		
		chrono::steady_clock::time_point sent_ping;
		chrono::steady_clock::time_point since;
		int mode; //0 tor; 1 ip4
		chrono::microseconds latency;
		stage_t stage;

	};

}
}

#endif

