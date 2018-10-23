#ifndef USGOV_a77bb134f65a2d28d9cf7823a14041036d606c371217ddd8b91b59f77a73b6b1
#define USGOV_a77bb134f65a2d28d9cf7823a14041036d606c371217ddd8b91b59f77a73b6b1

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

