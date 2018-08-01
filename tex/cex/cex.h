#ifndef CUREX_CEX_H
#define CUREX_CEX_H

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

//#define MONEY_LEAK_TEST


#ifdef __GNUC__
# if (__GNUC__ >= 3)
#  define likely(x)     __builtin_expect ((x), 1)
#  define unlikely(x)   __builtin_expect ((x), 0)
# else
#  define likely(x)     (x)
#  define unlikely(x)   (x)
# endif
#else
# define likely(x)      (x)
# define unlikely(x)    (x)
#endif

namespace curex {
namespace cex {

typedef unsigned long long id;
typedef wchar_t char_type;
typedef std::basic_string<char_type> string;
typedef std::basic_ostream<char_type> ostream;
typedef std::basic_istream<char_type> istream;
typedef std::basic_ostringstream<char_type> ostringstream;
typedef std::basic_istringstream<char_type> istringstream;
typedef std::basic_ofstream<char_type> ofstream;
typedef std::basic_ifstream<char_type> ifstream;



}}

#include "currencies.h"
#include "efp.h"

#endif
