#ifndef US_GOV_INPUT_CFG0_H
#define US_GOV_INPUT_CFG0_H

#include <string>

namespace us { namespace gov { namespace input {
using namespace std;

class cfg0 {
public:
    cfg0(const string& home);
    cfg0(const cfg0& other);
    virtual ~cfg0();

    static void check_platform();
    static bool is_big_endian();
    static bool mkdir(const string& d);
    static bool file_exists(const string& f);
    static bool ensure_dir(const string& d);
    static bool exists_dir(const string& d);
    static string abs_file(const string& home, const string& fn);
    static void mkdir_tree(string sub, string dir);
    static cfg0 load(const string& home);

    const string& get_home() const {
        return m_home;
    }
private:
    string m_home;
};

typedef cfg0 cfg_filesystem;

}
}}

#endif

