/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#ifndef US_GOV_INPUT_CONVERT_H
#define US_GOV_INPUT_CONVERT_H

#include <us/gov/crypto.h>

namespace us{ namespace gov { namespace input {
using namespace std;

/** @brief Convert a string s into the template type T
 *  @param[in] string object to convert
 *  @return template type of the input string
 *
 */
template<typename T>
T convert(const string& s) {
    T v;
    istringstream is(s);
    is >> v;
    return move(v);
}

/*! Template specialization to convert a string into a string type */
template<> string convert(const string& s);

/*! Template specialization to convert a string into a priv_t type */
template<> us::gov::crypto::ec::keys::priv_t convert(const string& s);

/*! Template specialization to convert a string into a pub_t type */
template<> us::gov::crypto::ec::keys::pub_t convert(const string& s);

}}}

#endif



