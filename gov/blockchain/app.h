#ifndef USGOV_ab4c3f8b32c28ebe389cd176419697be221f06c33dafc174ba9b3c3cf56ff3fe
#define USGOV_ab4c3f8b32c28ebe389cd176419697be221f06c33dafc174ba9b3c3cf56ff3fe

#include <gov/peer.h>
#include <gov/socket.h>


#include <vector>
#include <cassert>
#include <thread>
#include <chrono>
#include <gov/crypto.h>

namespace usgov {
namespace blockchain {
	using namespace std;
	using socket::datagram;

	struct peer_t;

	struct app_gut;
	struct app_gut2;
	struct block;
	struct pow_t;

	struct daemon;
	struct block;
	struct app {
		typedef crypto::ec::keys keys;
		app() {}
		virtual ~app() { }
		virtual int get_id() const=0;
		virtual string get_name() const=0;
		virtual void on_begin_cycle()=0;
		virtual app_gut* create_app_gut()=0;
		virtual bool process_work(peer_t *, datagram*)=0;
		virtual bool process_query(peer_t *, datagram*) { return false; }
		virtual bool process_evidence(peer_t *, datagram*) { return false; }
		virtual string shell_command(const string& cmdline)=0;

		typedef crypto::ripemd160 hasher_t;

		virtual void dbhash(hasher_t&) const=0;

		virtual void clear()=0;

		virtual void run()=0;

		virtual void import(const app_gut2&, const pow_t&)=0;

		const keys& get_keys() const;
		daemon* parent{0};
	};

	struct app_gut {
		virtual ~app_gut() { }

		static app_gut* create(int id);
		static app_gut* create(istream&);
		static app_gut* create(int id, istream& is);

		virtual void to_stream(ostream&) const=0;
		virtual void from_stream(istream&)=0;
		virtual int app_id() const=0;
	};

	struct app_gut2 {
		app_gut2() {
		}
		virtual ~app_gut2() { 
		}
		static app_gut2* create(int id);
		static app_gut2* create(istream&);
		static app_gut2* create(int id,istream&);

		virtual uint64_t merge(blockchain::app_gut* other) {
			++multiplicity;
			delete other;
			return 0;
		}
		virtual void end_merge()=0;
		virtual void to_stream(ostream&) const=0;

		unsigned long multiplicity{1}; //updated by merge

	};


	template<typename D, typename T>
	struct policies_base: array<D,T::num_params> {
		typedef array<D,T::num_params> b;
		policies_base() {
			for (auto&i:*this) i=0;
		}
		policies_base(const b& g):b(g) {}
		virtual ~policies_base() {}
		void to_stream(ostream& os) const {
			for (auto& i:*this) {
				os << i << " ";
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
	struct policies_app_gut: policies_base<D,T>, app_gut {
		typedef policies_base<D,T> b1;
		typedef app_gut b;
		policies_app_gut() {}
		policies_app_gut(const typename b1::b& g): b1(g) {
		}
		virtual ~policies_app_gut() {}
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
			v/=m;
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
	struct policies_app_gut2: policies_base<D,T>, app_gut2 {
		typedef policies_base<D,T> b1;
		typedef app_gut2 b;
		policies_app_gut2() {}
		policies_app_gut2(const policies_app_gut<D,T>& g): b1(g) {

		}
		virtual ~policies_app_gut2() {}

		virtual uint64_t merge(blockchain::app_gut* other0) override {
			b1* other=dynamic_cast<b1*>(other0);
			for (int i=0; i<T::num_params; ++i) merger.merge((*this)[i],(*other)[i]);  //(*this)[i]+=(*other)[i];
			b::merge(other0);
			return 0;
		}
		virtual void end_merge() override {
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
		b operator - (const policies_t& other) const {
			b ans;
			for (int i=0; i<T::num_params; ++i) {
				ans[i]=(*this)[i]-other[i];
			}
			return move(ans);
		}
		void hash(hasher_t& h) const {
			for (auto& i:*this) {
				h << i;
			}
		}

	};

	struct miner_work_value:unordered_map<int, int64_t> {
		int64_t value() const {
			int64_t v=0;
			for (auto&i:*this) {
				v+=i.second;
			}
			return v;
		}		
	};
}
}

#endif

