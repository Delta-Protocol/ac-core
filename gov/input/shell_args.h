#ifndef US_GOV_INPUT_SHELL_ARGS_H
#define US_GOV_INPUT_SHELL_ARGS_H

#include <string>
#include "convert.h"

namespace us{ namespace gov{ namespace input{
using namespace std;

class shell_args {
public:
    shell_args(int argc, char** argv):m_argc(argc), m_argv(argv) {}

    template<typename T>
    T next() {
        if (m_num>=m_argc) {
            return T();
        }
        string i=m_argv[m_num++];
        return convert<T>(i);
    }

    template<typename T>
    T next(const T& default_value) {
        if (m_num>=m_argc) {
            return default_value;
        }
        string i=m_argv[m_num++];
        return convert<T>(i);
    }

    int args_left() const {
        return m_argc-m_num;
    }

    int get_num() const {
        return m_num;
    }
 
    void set_num(int num){
        m_num = num; 
    }

private:
    int m_argc;
    char** m_argv;
    int m_num{1};
};

}}}

#endif
