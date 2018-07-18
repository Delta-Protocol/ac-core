#ifndef USGOV_2db53cabc8e424c213eaec41bfee681cd2c20ca24af646b25ed7950072018f45
#define USGOV_2db53cabc8e424c213eaec41bfee681cd2c20ca24af646b25ed7950072018f45

#include <string>
#include <unordered_map>
#include <us/gov/crypto.h>
#include <mutex>

namespace us { namespace wallet {
using namespace std;

struct pairing {
	typedef gov::crypto::ec::keys::pub_t pub_t;
	typedef pub_t::hash_t hash_t;

    pairing(const string& homedir);
    virtual ~pairing();

    struct device {
        device() {
        }
        device(const pub_t& pub, const string& name): pub(pub), name(name) {
        }
        device(const device& other): pub(other.pub), name(other.name) {
        }
        static string default_name;

        void to_stream(ostream&) const;
        static pair<bool,device> from_stream(istream&);
        void dump(ostream& os) const;

        string name;
        pub_t pub;
        //int seq{0}; //used to avoid replay attacks
    };

    struct devices_t: unordered_map<hash_t,device> {
        devices_t(const string& home);
        static string not_found;

        void load();
        void save() const;
        void load_(); //caller needs to lock mx
        void save_() const;  //caller needs to lock mx
        string file;
        mutable mutex mx;
        string home;

        bool authorize(const pub_t& p) const;

        void dump(ostream& os) const;

	string pair_request();
        void pair(const pub_t& pub, const string& name);
        void unpair(const pub_t& pub);
        const string& get_name(const pub_t& pub);

	string msg;
    };

    devices_t devices;
};

}
}

#endif

