#ifndef USGOV_c240798d9f2ba9cd92a0728e617b7d39e33fe0519df7744799c27d2312bc782b
#define USGOV_c240798d9f2ba9cd92a0728e617b7d39e33fe0519df7744799c27d2312bc782b

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <csignal>
#include <cassert>
#include <memory>
#include <mutex>
#include <us/gov/signal_handler.h>
#include <us/gov/crypto/ec.h>
#include "client.h"
#include "datagram.h"

namespace us { namespace gov {
namespace socket {
	struct datagram;
	struct server: signal_handler::callback {
		server();
		server(uint16_t port);
		virtual ~server();

		virtual client* create_client(int sock);

		virtual void on_finish() override;

		void run();

		datagram* read_from_client(int sock);
		int make_socket (uint16_t port);

		virtual void attach(client*,bool wakeupselect=true);
		virtual void detach(client*);

		uint16_t port;

        bool banned_throttle(const string& addr);

		virtual void receive_and_process(client*c) { c->busy.store(false); };
       	typedef crypto::ec::keys::pub_t pub_t;

		struct clients_t: unordered_map<int,client*> {
			typedef unordered_map<int,client*> b; ///<sock,client*>

			clients_t() {
            }
			clients_t(const clients_t& other)=delete;
			~clients_t();

			bool is_here(const client&) const;

//			struct rmlist:unordered_map<int,client*> {
//				typedef unordered_map<int,client*> b;
			struct rmlist:unordered_set<client*> {
				typedef unordered_set<client*> b;
				~rmlist();
				bool add(client* c);
				//bool remove(int fd);
				//bool find(int fd) const;
				void dump(ostream& os) const;
				mutable mutex mx;
			};

			struct wait:unordered_set<client*> {
				typedef unordered_set<client*> b;
				~wait();
				void add(client* c);
				bool remove(client* c);
				bool find(const client& c) const;
				void dump(ostream& os) const;
				mutable mutex mx;
			};
/*
			struct hold_t:unordered_set<client*> {
				typedef unordered_set<client*> b;
				~hold_t();
				void add(const client& c);
				bool remove(const client& c);
				bool find(const client& c) const;
				void dump(ostream& os) const;
				mutable mutex mx;
			};
*/
//			struct attic_t:unordered_map<client*,chrono::steady_clock::time_point> {
			struct attic_t:unordered_set<client*> {
				~attic_t();
                void add(client*);
				void purge(); //definitely delete those clients that terminated long ago, in hope there is no more workers on them
			};



			void add(client* c, bool wakeupselect=true);
			void remove(client* c);
//			void hold(client* c);
//			void resume(client* c);
			void grow();
			void shrink();
            vector<int> update();

			vector<client*> active() const { 
                lock_guard<mutex> lock(mx_active);
                return active_;
            }
            mutable mutex mx_active;
            vector<client*> active_;

			void dump(ostream& os) const;
		private:
			mutable mutex mx;
			wait wadd;
			rmlist wremove;
			//wait holds;
			attic_t attic;
		public:
			void read_sockets();
			client locli; //loopback;
		};

		void dump(ostream& os) const;
		vector<client*> active() const { return clients.active(); }

//		static unique_ptr<net::os> os;
		int sock{0};
		clients_t clients;
	};

}
}}

#endif

