#ifndef USGOV_75e4bd922f2de31a967ff5ad48fd3c21a0df7b241765114881ed8761ed8ca7cd
#define USGOV_75e4bd922f2de31a967ff5ad48fd3c21a0df7b241765114881ed8761ed8ca7cd

#include <us/gov/dfs/daemon.h>
#include <us/gov/peer/peer_t.h>
#include <us/gov/relay/peer_t.h>
#include <us/gov/socket/client.h>
#include <vector>
#include <unordered_set>
#include <string>

namespace us{ namespace gov {
namespace engine {
using namespace std;

        struct daemon;

		struct networking:dfs::daemon {
			typedef dfs::daemon b;
			using b::keys;
			networking(engine::daemon* parent);
			networking(uint16_t port, uint16_t edges, engine::daemon* parent, const vector<string>& seed_nodes);
			virtual bool process_work(socket::peer_t *c, datagram*d) override;
			virtual bool process_evidence(datagram*d) override;
			bool process_work_sysop(peer::peer_t *c, datagram*d);
			virtual string get_random_peer(const unordered_set<string>& exclude) const override; //returns ipaddress //there exist a possibility of returning "" even though there were eligible items available

            virtual const keys& get_keys() const override;
            virtual vector<relay::peer_t*> get_nodes() override;
			virtual socket::client* create_client(int sock) override;
			void dump(ostream& os) const;

			engine::daemon* parent;
			vector<string> seed_nodes;
		};

}}
}

#endif

