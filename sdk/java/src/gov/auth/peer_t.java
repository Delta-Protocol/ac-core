package us.gov.auth;

import us.gov.id.peer_t.h;


class peer_t extends id.peer_t {
        enum stage_t {
            denied=0,
            authorized,
            num_stages
        };
        constexpr static array<const char*,num_stages> stagestr={"denied","authorized"};

        virtual const keys& get_keys() const=0;

        virtual void verification_completed() override;

	virtual bool authorize(const pubkey_t& p) const=0;
 void dump(ostream& os) const;
                virtual void dump_all(ostream& os) const override {
                        dump(os);
                        b::dump_all(os);
                }

        //virtual string run_auth() override;

        peer_t(int sock=0);
        virtual ~peer_t();
        stage_t stage{denied};
};

}
}}

#endif

constexpr array<const char*,c::num_stages> c::stagestr;

c::peer_t(int sock):b(sock) {
}

c::~peer_t() {
}
 
void c::verification_completed() {
    b::verification_completed();
    if (!verification_is_fine()) {
        disconnect();
        return;
    }
   	if (!authorize(pubkey)) {
        disconnect();
        return;
   	}
   	stage=authorized;
}
void c::dump(ostream& os) const {
    os << this << ' ' << pubkey << "- " << stagestr[stage] << endl;
}

