#include "peer_t.h"
#include "daemon.h"
#include "protocol.h"

using namespace us::gov::peer;
using namespace std;
using namespace chrono_literals;

typedef us::gov::peer::peer_t c;

constexpr array<const char*,peer_t::num_stages> peer_t::stagestr;
constexpr array<const char*,2> peer_t::modestr;

c::peer_t(int sock): b(sock), stage(service)/*,latency(100ms)*/ {
/*
	if(sock!=0) {
		since=chrono::steady_clock::now();
	}
	else {
		stage=disconnected;
	}
*/
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
    b::on_connect();
//	since=chrono::steady_clock::now();
	//stage=connected;
	stage=service;
}


void c::disconnect() {
     {
     lock_guard<mutex> lock(mx);
     stage=disconnecting;
     }
     b::disconnect();
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
//	os << this << "- mode: " << modestr[mode] << "; age: " << age(since) << "; latency: " << latency.count() << "us; stage: " << stagestr[stage] << endl;
	os << this << "- mode: " << modestr[mode] << " stage: " << stagestr[stage] << endl;
}


