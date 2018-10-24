/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#include "convert.h"

using namespace us::gov::input;
using namespace std;

template<> string us::gov::input::convert(const string& s) {
    return s;
}

using us::gov::crypto::ec;

template<> ec::keys::priv_t us::gov::input::convert(const string& s) {
    return ec::keys::priv_t::from_b58(s);
}

template<> ec::keys::pub_t us::gov::input::convert(const string& s) {
    return ec::keys::pub_t::from_b58(s);
}


