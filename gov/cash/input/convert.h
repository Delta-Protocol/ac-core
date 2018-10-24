#ifndef US_GOV_CASH_INPUT_CONVERT_H
#define US_GOV_CASH_INPUT_CONVERT_H

#include <us/gov/input/convert.h>
#include <us/gov/cash/tx.h>

namespace us{ namespace gov {namespace input {

template<> us::gov::cash::tx::sigcode_t convert(const std::string& s);

}}}

#endif



