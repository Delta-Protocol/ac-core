#ifndef USGOV_adb8d14e20731e001ab4e0eb7742ff5881fc9a7fabb4666497868c91f9a18570
#define USGOV_adb8d14e20731e001ab4e0eb7742ff5881fc9a7fabb4666497868c91f9a18570

//#include <us/gov/peer/peer_t.h>
#include <us/gov/socket/peer_t.h>
#include <us/gov/crypto/crypto.h>
#include <us/gov/crypto/hash.h>

namespace us { namespace gov {
namespace id {

	using namespace std;

	struct daemon;
//	struct peer_t:peer::peer_t {
	struct peer_t:socket::peer_t {
//		typedef peer::peer_t b;
		typedef socket::peer_t b;
		typedef crypto::ec::keys keys;
		typedef keys::pub_t pubkey_t;
		typedef pubkey_t::hash_t pubkeyh_t;

		typedef keys::pub_t pub_t;
		using datagram=socket::datagram;

		enum stage_t { //coupled with sdk/java/src/Wallet.java
			anonymous=0,
			verified,
			verified_fail,
			num_stages
		};
		constexpr static array<const char*,num_stages> stagestr={"anonymous","verified","verified_fail"};

		peer_t(int sock);
		virtual ~peer_t();
		void dump(ostream& os) const;
                virtual void dump_all(ostream& os) const override {
                        dump(os);
                        b::dump_all(os);
                }

        bool process_work(datagram*d);

		void process_request(datagram* d, const keys&);
		void process_peer_challenge(datagram* d, const keys&);
		void process_challenge_response(datagram* d);
//		void process_peer_status(datagram* d);
		void initiate_dialogue();
		static string get_random_message();
		static string to_string(const vector<unsigned char>& data);

		//vector<unsigned char> sign(const keys::priv_t& pk, const string& text) const;
		//stage_t verify(const pubkey_t&, const string& signature) const;

        bool verification_is_fine() const { return stage_peer==verified; }

		virtual void verification_completed() {}

		daemon* get_parent() { return reinterpret_cast<daemon*>(parent); }

        virtual const keys& get_keys() const;
        virtual string run_auth_responder();

        virtual string connect(const string& host, uint16_t port, bool block=false) override;

		/// For every edge in the graph of nodes there are two nodes on each end, each node has an instance of this class, 
		/// If one of these nodes is me and the other is peer
		/// stage_me is my own stage (e.g. I am anonymous for him), stage_peer is the other end stage (e.g He is anonymous to me).
		stage_t stage_peer{anonymous};
//		stage_t stage_me{anonymous};

		string msg; //message sent to peer, to be signed by him to check his pubkey.
		pubkey_t pubkey;
	};

}
}}

#endif

