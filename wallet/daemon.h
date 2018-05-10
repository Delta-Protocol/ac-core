#ifndef USGOV_98e8c16336dfe2bb0f4b26ffe37d808f974893b94c081f2f0f0419f11ac5e0ae
#define USGOV_98e8c16336dfe2bb0f4b26ffe37d808f974893b94c081f2f0f0419f11ac5e0ae

#include <gov/socket/daemon.h>
#include <gov/crypto/ec.h>
#include "wallet.h"
#include <unordered_map>
#include "api.h"

namespace us { namespace wallet {

using namespace std;

using socket::datagram;

struct wallet_daemon: socket::daemon, local_api {
	typedef socket::daemon b;
	typedef socket::peer_t peer_t;
	typedef crypto::ec::keys::pub_t pub_t;
	typedef pub_t::hash_t hash_t;

	wallet_daemon(uint16_t port, const string& home, const string&backend_host, uint16_t backend_port);
	virtual ~wallet_daemon();
	bool process_work(peer_t*, datagram*);

  //  string backend_host;
  //  uint16_t backend_port;

	bool send_response(peer_t *c, datagram*d, const string& payload);

    struct device {
        device() {
        }
        device(const pub_t& pub, const string& name): pub(pub), name(name) {
        }

        void to_stream(ostream&) const;
        static device from_stream(istream&);

        string name;
        pub_t pub;
        int seq{0}; //used to avoid replay attacks
    };

    struct devices_t: unordered_map<hash_t,device> {
        devices_t(const string& home);
        void load();
        void save() const;
        void load_(); //caller needs to lock mx
        void save_() const;  //caller needs to lock mx
        static string default_name;
        string file;
        mutable mutex mx;
        string home;

        void pair(const pub_t& pub, const string& name);
        void unpair(const pub_t& pub);
        const string& get_name(const pub_t& pub);
    };

    devices_t devices;



};


}}

#endif

