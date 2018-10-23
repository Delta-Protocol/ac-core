#ifndef US_GOV_WALLET_PAIRING_H
#define US_GOV_WALLET_PAIRING_H

#include <string>
#include <unordered_map>
#include <mutex>

#include <us/gov/crypto.h>

namespace us{ namespace wallet{
using namespace std;

class pairing {
public:
    typedef gov::crypto::ec::keys::pub_t pub_t;
    typedef pub_t::hash_t hash_t;

    pairing(const string& homedir);
    virtual ~pairing();

    class device {
    friend class pairing; 
    public:
        device() {}
        device(const pub_t& pub, const string& name): m_pub(pub), m_name(name) {}
        device(const device& other): m_pub(other.m_pub), m_name(other.m_name) {}

        void to_stream(ostream&) const;
        static pair<bool,device> from_stream(istream&);
        void dump(ostream& os) const;

    private:
        static string m_default_name;
        string m_name;
        pub_t m_pub;
    };

    class devices_t: public unordered_map<hash_t,device> {
    public:
        devices_t(const string& home);

        void load();
        void save() const;
        //caller needs to lock mx  
        void load_();         

        //caller needs to lock mx
        void save_() const;  

        bool authorize(const pub_t& p) const;

        void dump(ostream& os) const;

	string pair_request() const;
        void pair(const pub_t& pub, const string& name);
        void unpair(const pub_t& pub);
        const string& get_name(const pub_t& pub) const;
    private:
        string m_file;
        mutable mutex m_mx;
        string m_home;
        static string m_not_found;
	string m_msg;
    };

public:
    const devices_t& get_devices() const{
        return m_devices;
    }

    void pair(const pub_t& pub, const string& name){
        m_devices.pair(pub, name);
    }

    void unpair(const pub_t& pub){
        m_devices.unpair(pub);
    }

private:
    devices_t m_devices;
};

}}

#endif

