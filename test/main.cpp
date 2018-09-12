#include <iostream>
#include "split_function.h" 		//dfs
#include "socket.h"            		//socket
#include <us/gov/socket/datagram.h>
#include <us/gov/crypto/symmetric_encryption.h>
#include <us/gov/crypto/ec.h>

#include "symmetric_encryption_test.h"
#include "elliptic_cryptography_test.h"



#include <us/gov/engine.h>
#include <thread>

using namespace std;
using namespace us::gov;

struct cli:engine::peer_t {

	cli(int s):engine::peer_t(s), k(crypto::ec::keys::generate()) {
	}

    cli(const keys& kk_):engine::peer_t(0), k(kk_) {
    }

    virtual void verification_completed() override {
        auth::peer_t::verification_completed();
    }

    virtual const keys& get_keys() const {
            return k;
    }

    keys k;
};

struct server: auth::daemon {

	server():auth::daemon(16666,10), k(crypto::ec::keys::generate()) {

	}

	virtual socket::client* create_client(int sock) override {
        return new cli(sock);
//        p->parent=this;
//        return p;
    }

	virtual const keys& get_keys() const {
		return k;
	}

	keys k;
};



int dup_node() {
    signal(SIGPIPE, SIG_IGN);

	server s;
	thread t(&server::run,&s);
	this_thread::sleep_for(1s);

	cli c1(0);
    string r=c1.connect("127.0.0.1",16666,true);

    if (r.empty()) {
        cout << "connected to address " << endl;
        cout << c1.run_auth_responder() << endl;

        for(int i=0; i<10; i++)
            cout << "sending dgram " << c1.send(new socket::datagram(protocol::ping)) << endl;

        cout << "okis" << endl;

    } else {

		cout << r << endl;
	}


	cli c2(c1.k);
    string r2=c2.connect("127.0.0.1",16666,true);

    if (r2.empty()) {

        cout << "connected to address " << endl;
        cout << c2.run_auth_responder() << endl;

        for(int i=0; i<10; i++)
            cout << "sending dgram " << c2.send(new socket::datagram(protocol::ping)) << endl;

        cout << "okis" << endl;

    } else {

		cout << r2 << endl;
	}

	t.join();
}

#include <us/gov/crypto/ec.h>
int main() {
/*
pubk:vrmLs7or2JtZ6w6umxf5w3Dn6PfWvMiWUkdgswjTjwZU
msg:21zqxoeTet7CG4oWrbPt1wRM48pL
signature:AN1rKvt2ovuVt9twTM13cP6ZBBQSUKEB3i86bxY1PQZZCFS9UjVdR5jLm4VwMZb1v5s7fQbEaoxB4c527VWDN1BBbPjKws7t5
*/
//    bool verify(const keys::pub_t& pk, const string& text, const string& signature_der) const;


auto pubk=us::gov::crypto::ec::keys::pub_t::from_b58("vrmLs7or2JtZ6w6umxf5w3Dn6PfWvMiWUkdgswjTjwZU");
string sig="AN1rKvt2ovuVt9twTM13cP6ZBBQSUKEB3i86bxY1PQZZCFS9UjVdR5jLm4VwMZb1v5s7fQbEaoxB4c527VWDN1BBbPjKws7t5";
string msg="21zqxoeTet7CG4oWrbPt1wRM48pL";


cout << us::gov::crypto::ec::instance.verify(pubk,msg,sig) << endl;
exit(0);



return 0;
	dup_node();
	test_symmetric_encryption();
	test_elliptic_cryptography();
	//testing_split_function(); 		//dfs 	 --works!
	testing_socket_datagram(); 		//socket --works!
	//testing_socket_communication(); 	//--errors--

	
	exit(0);
}


