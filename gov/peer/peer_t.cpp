#include "peer_t.h"
#include "daemon.h"
#include "protocol.h"

using namespace us::gov::peer;
using namespace std;
using namespace chrono_literals;

typedef us::gov::peer::peer_t c;

constexpr array<const char*,peer_t::num_stages> peer_t::stagestr;
constexpr array<const char*,2> peer_t::modestr;

c::peer_t(int sock): b(sock), stage(service),latency(100ms) {
	if(sock!=0) {
		since=chrono::steady_clock::now();
	}
	else {
		stage=disconnected;
	}
}

c::~peer_t() {
}

bool c::connect(const string& host, uint16_t port, bool block) {
	if (b::connect(host, port, block)) {
		return true;
	}
	return false;
}

void c::on_connect() {
cout << "PEER ONCONNECT" << endl;
    b::on_connect();
	since=chrono::steady_clock::now();
	//stage=connected;
	stage=service;
}


bool c::ping() {
	{
	lock_guard<mutex> lock(mx);
	sent_ping=chrono::steady_clock::now();
	}
	return send(protocol::ping,"ping").empty();
}


bool c::is_slow() const {
	using namespace std::chrono_literals;
	if (latency>chrono::steady_clock::duration(300ms)) return true; //TODO parameter
	return false;
}

void c::process_pong() {
	lock_guard<mutex> lock(mx);
	latency=chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now()-sent_ping);
	if (!is_slow()) {
		stage=service;
	}
	else {
		stage=exceed_latency; 
		return;
	}
}

void c::disconnect() {
     {
     lock_guard<mutex> lock(mx);
     stage=disconnecting;
     }
     b::disconnect();
}

bool c::process_work(datagram* d) { //executed by thread from pool 
     assert(d!=0);
     switch(d->service) {
         case protocol::ping: {
			delete d;
			send(protocol::pong,"pong");
			break;
		 }
         case protocol::pong: {
			delete d;
			process_pong();
			break;
		 }
		 default: return false;
     }
     return true;
}

#include <iomanip>
#include <sstream>
#include <tuple>

namespace {

	template <typename Container, typename Fun>
	void tuple_for_each(const Container& c, Fun fun) {
		for (auto& e : c)
		    fun(std::get<0>(e), std::get<1>(e), std::get<2>(e));
	}


	string duration_str(chrono::milliseconds time) {
	    using namespace std::chrono;
	    
	    using T = std::tuple<milliseconds, int, const char *>;
	    
	    constexpr T formats[] = {
		T{hours(1), 2, ""},
		T{minutes(1), 2, ":"},
		T{seconds(1), 2, ":"},
	    };
	    
	    std::ostringstream o;

	    tuple_for_each(formats, [&time, &o](auto denominator, auto width, auto separator) {
		o << separator << std::setw(width) << std::setfill('0') << (time / denominator);
		time = time % denominator;
	    });

	    return o.str();
	}

	string age(chrono::steady_clock::time_point time) {
		using namespace std::chrono;
		return duration_str(duration_cast<milliseconds>(steady_clock::now()-time));
	}

}

void c::dump(ostream& os) const {
	os << this << "- mode: " << modestr[mode] << "; age: " << age(since) << "; latency: " << latency.count() << "us; stage: " << stagestr[stage] << endl;
}


