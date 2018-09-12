#ifndef USGOV_ab4c3f8b32c28ebe389cd176419697be221f06c33dafc174ba9b3c3cf56ff3fe
#define USGOV_ab4c3f8b32c28ebe389cd176419697be221f06c33dafc174ba9b3c3cf56ff3fe

#include <us/gov/peer.h>
#include <us/gov/socket.h>
#include "evidence.h"

#include <vector>
#include <cassert>
#include <thread>
#include <chrono>
#include <mutex>
#include <us/gov/crypto.h>


namespace us { namespace gov {
namespace engine {
	using namespace std;
	using socket::datagram;

	struct peer_t;

	struct local_delta;
	struct delta;
	struct block;
	struct pow_t;

	struct daemon;
	struct block;
	struct app {
		typedef crypto::ripemd160 hasher_t;
		typedef hasher_t::value_type hash_t;

		struct local_delta { //an instance of local_delta serves as storage for processed transactions (mempool) in pre-consensus. It represents our vote on the right mempool
			virtual ~local_delta() {
                //cout << "engine::app::~local_delta" << endl;
            }

			static local_delta* create(int id);
			static local_delta* create(istream&);
			static local_delta* create(int id, istream& is);

			virtual void to_stream(ostream&) const=0;
			virtual void from_stream(istream&)=0;
			virtual int app_id() const=0;
		};

		struct delta { //an instance of delta merges local_deltas as they arrive (consensus). It represents the agreed change to the ledger
			delta() {}
			virtual ~delta() {}

			static delta* create(int id);
			static delta* create(istream&);
			static delta* create(int id,istream&);

			virtual uint64_t merge(local_delta* other);
			virtual void end_merge()=0;
			virtual void to_stream(ostream&) const=0;

			unsigned long multiplicity{0}; //counter for the number of local_deltas merged so far. This variable is updated by merge().
		};

		app() {}
		virtual ~app() {}

		virtual int get_id() const=0;
		virtual string get_name() const=0; //provide a name for your app

		virtual void clear()=0; //override and implement a db reset to a genesis state.
		virtual void dbhash(hasher_t&) const=0; //override and return the hash of the content of your db
		virtual void import(const delta&, const pow_t&)=0; //override and apply the given delta to your db

		virtual evidence* parse_evidence(uint16_t service, const string& datagram_payload) const = 0;
        virtual void process(const evidence&) = 0;

		virtual bool process_query(peer_t *, datagram*) { return false; } //override to respond to user's read-only queries

		virtual local_delta* create_local_delta()=0; // override to be able to submit your diff (your mempool)

		virtual string shell_command(const string& cmdline); //answers to shell commands

		static unsigned int get_seed();


        struct chaininfo_t {
            chaininfo_t():last_block_imported(0), tip(0) {
            }
            bool not_equals_tip(const hash_t& h) const {
                unique_lock<mutex> lock(mx_tip);
                return tip!=h;
            }
            const hash_t& get_tip() const {
                unique_lock<mutex> lock(mx_tip);
                return tip;
            }
            void set_tip(const hash_t& t) {
                unique_lock<mutex> lock(mx_tip);
                tip=t;
            }
    		hash_t last_block_imported; //no need to lock, write occurs during import and apps never read it during this time.
	    	hash_t tip; //need the lock, writes occur on voting result, reads occur every time on tx validation
            mutable mutex mx_tip;
        };

        static chaininfo_t chaininfo;
//        engine::daemon* parent;
	};

	struct runnable_app: app {
		virtual void run()=0;
	};



	template<typename D, typename T>
	struct policies_base: array<D,T::num_params> {
		typedef array<D,T::num_params> b;
		policies_base() {
			for (auto&i:*this) i=0;
		}
		policies_base(const b& g):b(g) {}
		virtual ~policies_base() {
            //cout << "~policies_base" << endl;
        }
		void to_stream(ostream& os) const {
			for (auto& i:*this) {
				os << i << ' ';
			}
		}
		void from_stream(istream& is) {
			for (int i=0; i<T::num_params; ++i) {
				double v;
				is >> v;
				(*this)[i]=v;
			}
		}
	};


	template<typename D, typename T>
	struct policies_local_delta: policies_base<D,T>, app::local_delta {
		typedef policies_base<D,T> b1;
		typedef local_delta b;
		policies_local_delta() {}
		policies_local_delta(const typename b1::b& g): b1(g) {
		}
		virtual ~policies_local_delta() {
            //cout << "~policies_local_delta" << endl;
        }
		virtual void to_stream(ostream& os) const override {
			b1::to_stream(os);
		}
		void from_stream(istream& is) {
			b1::from_stream(is);
		}

	};

	template<typename D>
	struct average_merger {
		void merge(D& v1, const D& v2) const {
			v1+=v2;
		}
		void end_merge(D& v, const unsigned long& multiplicity) const {
			double m=multiplicity;
//cout << "avg merger input:" << v << " multiplicity:" << m << " ret: ";
			v/=m;
//cout << v << endl;
		}
	};

	template<typename D>
	struct majority_merger {
		void merge(D&, const D& v2) {
			auto i=m.find(v2);
			if (i!=m.end()) {
				++i->second;
				return;
			}
			m.emplace(v2,1);

		}
		void end_merge(D& v, const unsigned long&) {
			const D* x=0;
			unsigned long z=0;
			for (auto& i:m) {
				if (i.second>z) {
					z=i.second;
					x=&i.first;
				}
			}
			if (likely(z!=0)) v=*x;
			else v=0;
			m.clear();
		}
		unordered_map<D,unsigned long> m;
	};

	template<typename D, typename T, typename M>
	struct policies_delta: policies_base<D,T>, app::delta {
		typedef policies_base<D,T> b1;
		typedef delta b;
		policies_delta() {}
		policies_delta(const policies_local_delta<D,T>& g): b1(g) {}
		virtual ~policies_delta() {}

		virtual uint64_t merge(app::local_delta* other0) override {
			b1* other=dynamic_cast<b1*>(other0); //TODO check if we can do static_cast
			for (int i=0; i<T::num_params; ++i) merger.merge((*this)[i],(*other)[i]);  //(*this)[i]+=(*other)[i];
//cout << "about to delete app::local_delta" << endl;
			b::merge(other0);
//cout << "done" << endl;
			return 0;
		}
		virtual void end_merge() override {
//cerr << "policies delta: end merge with multiplicity=" << b::multiplicity << endl;

			double m=b::multiplicity;
			for (int i=0; i<T::num_params; ++i) merger.end_merge((*this)[i],m); //(*this)[i]/=m;
		}
		M merger;
	};

	template<typename D, typename T>
	struct policies_t: array<D,T::num_params>, T {
		typedef array<D,T::num_params> b;
		typedef app::hasher_t hasher_t;

		policies_t() {
			clear();
		}
		virtual ~policies_t() {}

        void clear() {
			for (auto& i:*this) i=0;
        }

		policies_t& operator =(const policies_t& other) {
			for (int i=0; i<T::num_params; ++i) {
				(*this)[i]=other[i];
			}
			return *this;
		}

		b operator -(const policies_t& other) const {
			b ans;
			for (int i=0; i<T::num_params; ++i) {
				ans[i]=(*this)[i]-other[i];
			}
			return move(ans);
		}

		void hash(hasher_t& h) const {
			for (auto& i:*this) h << i;
		}
	};
/*
	struct miner_work_value:unordered_map<int, int64_t> {
		int64_t value() const {
			int64_t v=0;
			for (auto&i:*this) {
				v+=i.second;
			}
			return v;
		}		
	};
*/
}}
}
#endif

