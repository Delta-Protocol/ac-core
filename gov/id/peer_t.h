#ifndef USGOV_adb8d14e20731e001ab4e0eb7742ff5881fc9a7fabb4666497868c91f9a18570
#define USGOV_adb8d14e20731e001ab4e0eb7742ff5881fc9a7fabb4666497868c91f9a18570

#include <us/gov/socket/peer_t.h>
#include <us/gov/crypto/crypto.h>
#include <us/gov/crypto/hash.h>

namespace us{ namespace gov{ namespace id{
using namespace std;

struct daemon;

class peer_t: public socket::peer_t {
public:
    typedef crypto::ec::keys keys;
    typedef keys::pub_t pubkey_t;
    typedef pubkey_t::hash_t pubkeyh_t;

    typedef keys::pub_t pub_t;
    using datagram=socket::datagram;

    enum stage_t {
        anonymous=0,
        verified,
        verified_fail,
        num_stages
    };

public:
    peer_t(int sock);
    virtual ~peer_t();

    const pubkey_t& get_pubkey()const{
        return m_pubkey;
    }

public:    
    void dump(ostream& os) const;
    virtual void dump_all(ostream& os) const override {
        dump(os);
        socket::peer_t::dump_all(os);
    }

    bool process_work(datagram*d);

    static string get_random_message();

public:
    virtual const keys& get_keys() const;
    virtual string run_auth_responder();
    virtual string connect(const string& host, 
                           uint16_t port, 
                           bool block=false) override;

public:
    bool verification_is_fine() const { 
        return m_stage_peer==verified; 
    }

protected:
    void set_pubkey(const pubkey_t& pk){
        m_pubkey = pk;
    }

private:
    void process_request(datagram* d, const keys&);
    void process_peer_challenge(datagram* d, const keys&);
    void process_challenge_response(datagram* d);

private:
    void initiate_dialogue();
    
    static string to_string(const vector<unsigned char>& data);

private:
    virtual void verification_completed() {}

    daemon* get_parent() { 
        return reinterpret_cast<daemon*>(parent); 
    }

private:
    constexpr static array<const char*,num_stages> stagestr={"anonymous","verified","verified_fail"};

private:
    stage_t m_stage_peer{anonymous};
    string m_msg;
    pubkey_t m_pubkey;
};

}}}

#endif

