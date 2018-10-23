#ifndef US_GOV_ENGINE_SHELL_H
#define US_GOV_ENGINE_SHELL_H

#include <cstdint>
#include <string>
#include <iostream>

namespace us{ namespace gov{ namespace engine{
using namespace std;
class daemon;

class shell {
public:
    shell(daemon&d):m_d(d) {}
    void help(ostream&) const;
    string command(const string& cmd);

private:
    daemon& m_d;
    int m_level{7};
    int m_cur_app{-1};
    string m_memory_cmd_lvl; 
};

}}}

#endif

