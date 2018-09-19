#ifndef USGOV_1a04abafd244aa56917030342d01e6017e089433f96dd01ee598fed0d91162dd
#define USGOV_1a04abafd244aa56917030342d01e6017e089433f96dd01ee598fed0d91162dd

#include <us/gov/relay.h>
#include <iostream>
#include <vector>

namespace us { namespace gov {
namespace dfs { //distributed file system
	using namespace std;
	typedef relay::peer_t peer_t;
	using socket::datagram;

	struct daemon:relay::daemon {
		typedef relay::daemon b;

		daemon(const string& home): homedir(home+"/dfs") {}
		daemon(uint16_t port, uint16_t edges, const string& home): b(port,edges), homedir(home+"/dfs") {}
		virtual ~daemon() {}

		virtual bool process_work(socket::peer_t *c, datagram*d) override;
        void dump(ostream&) const;

		void save(const string& hash, const vector<uint8_t>& data, int propagate);  //-1 nopes, 0=all peers; n num of peers
		string load(const string& hash);

		void request(peer_t *c, datagram*d);
		void response(peer_t *c, datagram*d);

		static string resolve_filename(const string& filename);

		string homedir;
		unordered_map<string, condition_variable*> file_cv;
	};

}

}}

#endif

