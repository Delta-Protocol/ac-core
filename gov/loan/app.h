/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#ifndef US_GOV_LOAN_APP_H
#define US_GOV_LOAN_APP_H

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

/*!
 * \class local_delta
 *
 * \brief define local changes to serialize to on a given node. It can be seen
 *        like transaction collected by a node.
 *
 */
class local_delta:public engine::policies_local_delta<hash_t, policies_traits> {
private:
    typedef engine::policies_local_delta<hash_t, policies_traits> base;

protected:
    virtual int app_id() const override;
    virtual void to_stream(ostream&) const override;
    virtual void from_stream(istream&) override;

public:

    /** @brief Default constructor
     *
     */
    local_delta() {};

    /** @brief Virtaul destructor
     *
     */
    virtual ~local_delta() {}

    /** @brief Transaction fees applied when merging imported deltas
     *
     */
    cash_t m_fees{0};
};

/*!
 * \class local_delta
 *
 * \brief define imported deltas and the policy to merge them. By default the
 *        policy to merge is based on a majority_merger which merges the hashes
 *        with the highest frequency
 *
 */
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

    /** @brief Default constructor
     *
     */
    delta() {}

    /** @brief Virtaul destructor
     *
     */
    virtual ~delta() {}

    /** @brief Read incoming deltas
     *  @param[in] is input stream to read from
     *  @return pointer to delta object
     *
     */
    static delta* from_stream(istream& is);

    /** @brief Transaction fees applied when merging imported deltas
     *
     */
    cash_t m_fees{0};
};

/*!
 * \class app
 *
 * \brief define loan application which inherit from engine::app that manges
 *        all the operations on deltas
 *
 */
class app: public engine::app {

private:

    loan::local_delta* m_pool{0};
    mutex m_mx_pool;

    class policies_t: public engine::policies_t<hash_t, policies_traits> {

    public:

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

    /** @brief Define name of the loan app
     *
     */
    constexpr static const char* name={"rep"};

    /** @brief Get identifier of load app (hard coded to 40)
     *  @return int identifier of loan app
     */
    static int id() { return 40; }

    /** @brief Default constructor
     *
     *  Initialise a new local delta and the associated policy for
     *  merging the local deltas
     *
     */
    app();

    /** @brief Virtual destructor
     *
     *  Delete instantiated local delta
     */
    virtual ~app();
};

}}}

#endif

