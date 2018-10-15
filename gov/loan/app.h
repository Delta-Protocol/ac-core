#ifndef USGOV_51d4393eec61d2ef445fd79c7b25d5d8a548fe50d2c4e798f97944ce8faf3208
#define USGOV_51d4393eec61d2ef445fd79c7b25d5d8a548fe50d2c4e798f97944ce8faf3208

#include <array>
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>

#include "us/gov/engine/app.h"
#include "us/gov/crypto/ripemd160.h"

namespace us { namespace gov { namespace engine {
class local_deltas;
}}}

namespace us { namespace gov { namespace loan {
using namespace std;

using socket::datagram;
using engine::local_deltas;
using crypto::ripemd160;
using engine::peer_t;

typedef int64_t cash_t;
typedef crypto::ripemd160 hasher_t;
typedef hasher_t::value_type hash_t;

class policies_traits {
public:
    enum paramid {
        next_patch=0,
        num_params,
    };

    constexpr static array<const char*,num_params> paramstr={"next_patch"};
};

class local_delta: public engine::policies_local_delta<hash_t, policies_traits> {
public:
    typedef engine::policies_local_delta<hash_t, policies_traits> b;

    local_delta() {}
    virtual ~local_delta() {}

    virtual int app_id() const override;
    virtual void to_stream(ostream&) const override;
    virtual void from_stream(istream&) override;

    cash_t fees{0};
};

class delta: public engine::policies_delta<hash_t, policies_traits, engine::majority_merger<hash_t>> {
public:
    typedef engine::policies_delta<hash_t, policies_traits, engine::majority_merger<hash_t>> b;

    delta() {}
    virtual ~delta() {}

    virtual uint64_t merge(engine::app::local_delta* other0) override {
        local_delta* other=static_cast<local_delta*>(other0);
        auto val=other->fees;
        b::merge(other0);
        return val;
    }

    virtual void to_stream(ostream& os) const override;
    static delta* from_stream(istream& is);
    cash_t fees{0};
};

class app: public engine::app {
public:
    constexpr static const char* name={"rep"};
    static int id() { return 40; }

    app();
    virtual ~app();

    virtual string get_name() const override { return name; }
    virtual int get_id() const override { return id(); }
    virtual string shell_command(const string& cmdline) override;
    virtual engine::app::local_delta* create_local_delta() override;
    virtual void import(const engine::app::delta&, const engine::pow_t&) override;
    virtual void dbhash(hasher_t&) const override;

    double supply_function(double x0, double x, double xf) const;
    void add_policies();

    loan::local_delta* pool{0};
    mutex mx_pool;

    class db_t {
    public:
        typedef ripemd160 hasher_t;
        typedef hasher_t::value_type hash_t;
        typedef unordered_map<hash_t,cash_t> accounts_t;

        db_t() {}

        void dump(ostream& os) const;
        cash_t get_newcash();

        mutable mutex mx;
    };
    db_t db;


    class policies_t: public engine::policies_t<hash_t, policies_traits> {
    public:
        typedef engine::policies_t<hash_t, policies_traits> b;

        policies_t() { temp_load(); }
        void temp_load() { (*this)[next_patch]=0; }
    };
    policies_t policies;
    policies_t policies_local;

    void dump_policies(ostream& os) const;

    mutable mutex mx_policies;
};

}}}

#endif

