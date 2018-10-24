#ifndef US_GOV_WALLET_WALLET_H
#define US_GOV_WALLET_WALLET_H

#include <unordered_map>
#include <fstream>

#include <us/gov/cash.h>
#include <us/gov/crypto.h>
#include <us/gov/auth.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/crypto.h>

#include "wallet_api.h"

namespace us{ namespace wallet{
using namespace std;
using namespace us::gov;

class wallet: public unordered_map<cash::hash_t,crypto::ec::keys> {
public:
    wallet(const string& datapath, 
           const string& backend_host, 
           uint16_t backend_port);
    virtual ~wallet();

    typedef cash::hash_t addr_t;
    typedef cash::cash_t cash_t;
    typedef cash::app::account_t account_t;


    class accounts_query_t: public cash::app::accounts_t {
    public:
        void dump(ostream& os) const;
    private:
        engine::diff::hash_t m_parent_block;
    };

    class input_account_t: public cash::app::account_t {
    friend class wallet;
    public:
        typedef cash::hash_t hash_t;
        typedef cash::cash_t cash_t;
        input_account_t(const hash_t& address,
                        const cash::app::account_t& acc, 
                        const cash_t& withdraw_amount);
        void dump(ostream& os) const;
    private:
        hash_t m_address;
        cash_t m_withdraw_amount;
    };

    class input_accounts_t: public vector<input_account_t> {
    public:
        cash::cash_t get_withdraw_amount() const;
        void dump(ostream& os) const;
    private:
        engine::diff::hash_t m_parent_block;
    };

    typedef wallet_api::tx_make_p2pkh_input tx_make_p2pkh_input;

    string filename() const;
    bool file_exists() const;
    bool load();
    bool save() const;

    cash::hash_t new_address();
    cash::hash_t add_address(const crypto::ec::keys::priv_t& key);
    const crypto::ec::keys* get_keys(const cash::hash_t& address) const;
    cash::cash_t balance() const;
    void dump_balances(ostream& os) const;
    void extended_balance(ostream& os) const;

    pair<string,accounts_query_t> query_accounts(socket::peer_t& peer, 
                           const cash::app::query_accounts_t& addresses) const;

    string refresh(socket::peer_t&);
 
    pair<string,input_accounts_t> select_sources(socket::peer_t& peer, 
                                                 const cash::cash_t& amount);

    string generate_locking_program_input(const crypto::ec::sigmsg_hasher_t::value_type& msg, 
                                          const cash::tx::sigcodes_t& sigcodes, 
                                          const cash::hash_t& address, 
                                          const cash::hash_t& locking_program);

    string generate_locking_program_input(const cash::tx& t, size_t this_index, 
                                          const cash::tx::sigcodes_t& sigcodes, 
                                          const cash::hash_t& address, 
                                          const cash::hash_t& locking_program);

    string send(socket::peer_t&, const cash::tx&) const;

    pair<string,unique_ptr<cash::tx>> tx_make_p2pkh(socket::peer_t&, 
                                                    const tx_make_p2pkh_input& i);
    pair<string,unique_ptr<cash::tx>> tx_sign(socket::peer_t&, const string& txb58, 
                                                  const cash::tx::sigcode_t& sigcodei, 
                                                  const cash::tx::sigcode_t& sigcodeo);

    void dump(ostream& os) const;
    void list(bool showpriv, ostream& os) const;

protected:
    uint16_t m_backend_port;
    string m_backend_host;

private:
    mutable bool m_need_save{false};
    accounts_query_t m_data;
    string m_datapath;
};

}}

#endif

