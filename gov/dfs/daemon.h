#ifndef USGOV_1a04abafd244aa56917030342d01e6017e089433f96dd01ee598fed0d91162dd
#define USGOV_1a04abafd244aa56917030342d01e6017e089433f96dd01ee598fed0d91162dd

#include <us/gov/relay.h>
#include <iostream>
#include <vector>

namespace us{ namespace gov{ namespace dfs{ 
using namespace std;
typedef relay::peer_t peer_t;

class daemon:public relay::daemon {
public:
    daemon(const string& home): m_homedir(home+"/dfs") {}
    daemon(uint16_t port, uint16_t edges, const string& home): 
            relay::daemon(port,edges), m_homedir(home+"/dfs") {}
    
    virtual ~daemon() {}

    virtual bool process_work(socket::peer_t *c, socket::datagram*d) override;

public:
    void dump(ostream&) const;

    void save(const string& hash, const string& data);

public:
    string load(const string& hash_b58, 
                condition_variable * pcv, bool file_arrived);
    string load(const string& hash_b58);

public:
    string get_path_from(const string& hash_b58, bool create_dirs=false) const;

    bool exists(const string& hash_b58) const;

    bool remove(const string& hash_b58);

    virtual void daemon_timer();

private:
    void request(relay::peer_t *c, socket::datagram*d);
    void response(relay::peer_t *c, socket::datagram*d);

    static string resolve_filename(const string& filename);

    virtual relay::peer_t* get_random_edge() const = 0;
    virtual relay::peer_t* get_random_edge(const relay::peer_t* exclude) const = 0;

public:
    struct params {
        condition_variable * pcv;
        chrono::system_clock::time_point time;
        bool file_arrived;
    };

    struct file_cv_t:unordered_map<string, params> {
        file_cv_t()  {}
        ~file_cv_t() {}

        bool exists(const string& hash_b58);

        void add(const string& hash_b58, condition_variable * pcv, bool file_arrived);
        void notify_and_erase(const string& hash_b58);
        void remove(const string& hash_b58);
        void wait_for(const string& hash_b58);
        void purge();

        mutex mx;
    };

    struct recents_t:unordered_map<string,chrono::system_clock::time_point> {
        recents_t()  {}
        ~recents_t() {}

        bool exists(const string& hash_b58);
        void add(const string& hash_b58);
        void purge();

        mutex mx;
    };

private:    
    string m_homedir;
    file_cv_t m_file_cv;
    recents_t m_recents;
};
}}}

#endif

