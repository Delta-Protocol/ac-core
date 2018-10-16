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

using engine::local_deltas;
using engine::peer_t;
using crypto::ripemd160;
using socket::datagram;

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

class local_delta:public engine::policies_local_delta<hash_t, policies_traits> {

protected:

    virtual int app_id() const override;
    virtual void to_stream(ostream&) const override;
    virtual void from_stream(istream&) override;

public:

    local_delta() {}
    virtual ~local_delta() {}

    cash_t m_fees{0};
};

class delta: public engine::policies_delta<hash_t,
                                           policies_traits,
                                           engine::majority_merger<hash_t>> {

private:

    typedef engine::policies_delta<hash_t,
                                   policies_traits,
                                   engine::majority_merger<hash_t>> base_t;

protected:

    virtual uint64_t merge(engine::app::local_delta* other0) override;
    virtual void to_stream(ostream& os) const override;

public:

    delta() {}
    virtual ~delta() {}

    static delta* from_stream(istream& is);

    cash_t m_fees{0};
};

class app: public engine::app {

private:

    loan::local_delta* m_pool{0};
    mutex m_mx_pool;

    class policies_t: public engine::policies_t<hash_t, policies_traits> {

    public:

        typedef engine::policies_t<hash_t, policies_traits> b;

        policies_t() { temp_load(); }
        void temp_load() { (*this)[next_patch]=0; }
    };

    void add_policies();
    void dump_policies(ostream& os) const;

    policies_t m_policies;
    policies_t m_policies_local;
    mutable mutex m_mx_policies;

protected:

    virtual string get_name() const override { return name; }
    virtual int get_id() const override { return id(); }
    virtual string shell_command(const string& cmdline) override;
    virtual engine::app::local_delta* create_local_delta() override;
    virtual void import(const engine::app::delta&, const engine::pow_t&) override;
    virtual void dbhash(hasher_t&) const override;

public:

    constexpr static const char* name={"rep"};
    static int id() { return 40; }

    app();
    virtual ~app();
};

}}}

#endif

