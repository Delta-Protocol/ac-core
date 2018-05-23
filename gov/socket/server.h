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
#include <us/gov/net.h>
#include "client.h"
#include "datagram.h"

namespace us { namespace gov {
namespace socket {
	struct datagram;
	struct server: signal_handler::callback {
		server();
		server(uint16_t port);
		virtual ~server();

		virtual client* create_client(int sock)=0;

		virtual void on_finish() override;

		void run();

		datagram* read_from_client(int sock);
		int make_socket (uint16_t port);
		void disconnect(client*);
		void incorporate(client*); //keep listening to this client who is already connected

		uint16_t port;

		struct pub_t:vector<client*> {
		};

		virtual bool receive_and_process(client*);

		struct clients_t: unordered_map<int,client*> {
			typedef unordered_map<int,client*> b; ///<sock,client*>

			clients_t(){}
			clients_t(const clients_t& other)=delete;
			~clients_t();

			bool is_here(client*) const;

			struct rmlist:unordered_map<int,client*> {
				typedef unordered_map<int,client*> b;
				~rmlist();
				void add(client* c);
				bool remove(int fd);
				bool find(int fd) const;
				void dump(ostream& os) const;
				mutable mutex mx;
			};

			struct wait:unordered_set<client*> {
				typedef unordered_set<client*> b;
				~wait();
				void add(client* c);
				bool remove(client* c);
				bool find(client* c) const;
				void dump(ostream& os) const;
				mutable mutex mx;
			};

			struct attic_t:unordered_map<client*,chrono::steady_clock::time_point> {
				~attic_t();
				void purge(); //definitely delete those clients that terminated long ago, in hope there is no more workers on them
			};

			void add(client* c,bool wakeupselect=true);
			void remove(client* c);
			void hold(client* c);
			void resume(client* c);
			void import();
			void finish();
			vector<int> update();
			pub_t active() const;
			void dump(ostream& os) const;
		private:
			mutable mutex mx;
			wait wadd;
			rmlist wremove;
			wait holds;
			attic_t attic;
		public:
			void read_sockets();
			client locli; //loopback;
		};

		void dump(ostream& os) const;
	
		pub_t active() const { return clients.active(); }
		static unique_ptr<net::os> os;
		int sock{0};
		clients_t clients;
	};

}
}}

#endif

