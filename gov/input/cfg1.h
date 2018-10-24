/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#ifndef US_GOV_INPUT_CFG1_H
#define US_GOV_INPUT_CFG1_H

#include <string>
#include <utility>

#include "us/gov/crypto/ec.h"
#include "cfg0.h"

namespace us { namespace gov { namespace input {
using namespace std;

/*!
 * \class cfg1
 *
 * \brief define methods to load and store keys. Extend functionalities of cfg0
 *
 */
class cfg1: public cfg0 {
public:

   /** @brief Alias for keys class
    *
    */
    typedef crypto::ec::keys keys_t;

    /** @brief Constructor
     *  @param[in] privk private key to initialise a node
     *  @param[in] home home directory
     *
     */
    cfg1(const keys_t::priv_t& privk, const string& home);

    /** @brief Copy constructor
     *
     */
    cfg1(const cfg1& other);

    /** @brief Virtual destructor
     *
     */
    virtual ~cfg1();

    /** @brief load private key of the node from the file system
     *  @param[in] home parent folder where the private key file is located
     *  @return pair with a flag set to true and the key (in b58) if it can be
     *          retrieved from the file system. Otherwise false with
     *          the key set to zero.
     */
    static pair<bool,keys_t::priv_t> load_priv_key(const string& home);

    /** @brief get path to file containing private key
     *  @param[in] home parent folder where the private key file is located
     *  @return string with the path to the local file
     */
    static string k_file(const string& home);

    /** @brief store private key  to file system
     *  @param[in] home parent folder where the private key file is located
     *  @param[in] priv private key to store (it will be store in b58 format)
     *  @return Void
     *
     */
    static void write_k(const string& home, const keys_t::priv_t& priv);
 
    /** @brief load private key and initialise base class
     *  @param[in] home parent folder where the private key file is located
     *  @return object of type cfg1
     *  @see cfg0::load
     *
     */
    static cfg1 load(const string& home);

    /** @brief get keys
     *  @return public member object containing public and private keys objects
     */
    const keys_t& get_keys() const {
        return m_keys;
    }

private:
    keys_t m_keys;
};

/** @brief Alias for cfg1 class
 *
 */
typedef cfg1 cfg_id;

}}}

#endif

