/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#ifndef US_GOV_INPUT_SHELL_ARGS_H
#define US_GOV_INPUT_SHELL_ARGS_H

#include <string>
#include "convert.h"

namespace us { namespace gov { namespace input {
using namespace std;

/*!
 * \class shell_args
 *
 * \brief wrapper that provides utilities to parse input parameters
 *
 */
class shell_args {
public:

    /** @brief Constructor
     *  @param[in] argc number of parameters expected to read
     *  @param[in] argv array of pointers to parameters
     *
     */
    shell_args(int argc, char** argv):m_argc(argc), m_argv(argv) {}

    /** @brief Read next parameter from input list
     *  @return parameter read from input
     *
     *  Generic method that converts the parameter to read to the template type.
     *  If there are not more arguments to read then it return a
     *  default constructed one.
     */
    template<typename T>
    T next() {
        if (m_num>=m_argc) {
            return T();
        }
        string i=m_argv[m_num++];
        return convert<T>(i);
    }

    /** @brief Read next parameter from input list
     *  @param[in] default_value value to assign if there are not more args to read
     *  @return parameter read from input
     */
    template<typename T>
    T next(const T& default_value) {
        if (m_num>=m_argc) {
            return default_value;
        }
        string i=m_argv[m_num++];
        return convert<T>(i);
    }

    /** @brief Constructor
     *  @param[in] argc number of parameters expected to read
     *  @param[in] argv array of pointers to parameters
     *
     */
    int args_left() const {
        return m_argc-m_num;
    }

    int m_argc;
    char** m_argv;
    int m_num{1};
};

}}}

#endif
