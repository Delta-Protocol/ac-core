/*

   public static final short wallet_base = 0;
    public static final short protocol_balance_query = wallet_base+1;
    public static final short protocol_dump_query = wallet_base+2;
    public static final short protocol_new_address_query = wallet_base+3;
    public static final short protocol_add_address_query = wallet_base+4;
    public static final short protocol_tx_make_p2pkh_query = wallet_base+5;
    public static final short protocol_tx_sign_query = wallet_base+6;
    public static final short protocol_tx_send_query = wallet_base+7;
    public static final short protocol_tx_decode_query = wallet_base+8;
    public static final short protocol_tx_check_query = wallet_base+9;
    public static final short protocol_pair_query = wallet_base+10;
    public static final short protocol_unpair_query = wallet_base+11;
    public static final short protocol_list_devices_query = wallet_base+12;

    public static final short protocol_response = wallet_base+0;
*/

#include <iostream>
#include <vector>
#include <sstream>
#include <cassert>
#include <fstream>
#include <functional>
using namespace std;

string file_prefix="apitool_generated__";

void include_snippet(const string& file,ostream&os) {
    ifstream f(file);
    while(f.good()) {
        string line;
        getline(f,line);
        os << line << endl;
    }
}
/*
///CONSUMED
void gen_cpp(const vector<string>& v, ostream& os) {
    os << "//------------------generated by apitool- do not edit" << endl;
    int n=0;
    const string& base=v[0];
    os << "    static constexpr uint16_t " << v[n++] << "{0};" << endl;
    for (;n<v.size();++n) {
        os << "    static constexpr uint16_t " << v[n] << "{" << base << "+" << n << "};" << endl;
    }
    os << "//-/----------------generated by apitool- do not edit" << endl;
}

//CONSUMED
void gen_java(const vector<string>& v, ostream& os) {
    os << "//------------------generated by apitool- do not edit" << endl;
    int n=0;
    const string& base=v[0];
    os << "    public static final short " << v[n++] << " = 0;" << endl;
    for (;n<v.size();++n) {
        os << "    public static final short protocol_" << v[n] << " = " << base << "+" << n << ";" << endl;
    }
    os << "//-/----------------generated by apitool- do not edit" << endl;
}

//consumed
void gen_gov_cpp(int mbase, const vector<string>& v, ostream& os) {
    os << "//------------------generated by apitool- do not edit" << endl;
    int n=0;
    const string& base=v[0];
    os << "    static constexpr uint16_t " << v[n++] << "{" << mbase << "};" << endl;
    for (;n<v.size();++n) {
        os << "    static constexpr uint16_t " << v[n] << "{((" << base << "+" << n-1 << ")<<2)+protocol_suffix};" << endl;
    }
    os << "//-/----------------generated by apitool- do not edit" << endl;
}
//consumed
void gen_gov_java(int mbase, const vector<string>& v, ostream& os) {
    os << "//------------------generated by apitool- do not edit" << endl;
    int n=0;
    const string& base=v[0];
    os << "    public static final short gov_" << v[n++] << " = " << mbase << ";" << endl;
    for (;n<v.size();++n) {
        os << "    public static final short gov_" << v[n] << " = ((gov_" << base << "+" << n-1 << ")<<2)+protocol_suffix;" << endl;
    }
    os << "//-/----------------generated by apitool- do not edit" << endl;
}
*/
/*
void gen_gov(int base, const vector<string>& v, ostream& os) {
    cout << "C++" << endl;
    gen_gov_cpp(base,v,os);
    cout << "files affected:" << endl;
    cout << "  gov/id/protocol.h" << endl;
    cout << endl;

    cout << "java" << endl;

    gen_gov_java(base,v,os);
    cout << "files affected:" << endl;
    cout << "  sdk/java/src/wallet.h" << endl;
    cout << endl;

}
*/

/*
int protocol_main() {

vector<string> f{ "wallet_base","balance_query","list_query",
    "new_address_query","add_address_query","tx_make_p2pkh_query",
    "tx_sign_query","tx_send_query","tx_decode_query",
    "tx_check_query","pair_query","unpair_query","list_devices_query",
    "ping","response" };


vector<string> gov_id{
    "id_base",
    "id_request",
    "id_peer_challenge",
    "id_challenge_response",
    "id_peer_status"
};


    cout << "-------------wallet" << endl;
   
    cout << "C++" << endl;
    gen_cpp(f,cout);
    cout << "files affected:" << endl;
    cout << "  wallet/protocol.h" << endl;
    cout << endl;

    cout << "java" << endl;
    gen_java(f,cout);
    cout << "files affected:" << endl;
    cout << "  sdk/java/src/Wallet.java" << endl;


    cout << "-------------gov" << endl;
    cout << "" << endl;
    cout << "-------------gov::id" << endl;
    gen_gov(200,gov_id,cout);

    return 0;
}

*/



    struct f: vector<string>  {
        typedef vector<string> args;
        string name;
        string fcgi;
        static f from_stream(istream& is) {
            f r;        
            int n;
            is >> r.name;
            is >> n;
            if (r.name.empty()) {
                return r;
            }
            string dummy;
            getline(is,dummy);    
            r.reserve(n);
            for (int i=0; i<n; ++i) {
                string line;
                getline(is,line);    
                if (!line.empty()) {
                    r.push_back(line);
                }
            }
            getline(is,r.fcgi);
            r.push_back("ostream&");
            return r;
        }

        void get_protocol_vector(const string& prefix, vector<string>& r) const {
            ostringstream os;
            os << prefix << '_' << name;
            r.push_back(os.str());
        }

        void gen_cpp_purevir(ostream& os) const {
            os << "  virtual void " << name << "(";
            if (!empty()) {
                auto e=end();
                --e;
                for (auto i=cbegin(); i!=e; ++i) {
                    os << *i << ", ";
                }
               os << *rbegin();
            }
           os << ")=0;" << endl;
            
        }
        void gen_cpp_override(ostream& os) const {
            os << "  virtual void " << name << "(";
            if (!empty()) {
                auto e=end();
                --e;
                for (auto i=cbegin(); i!=e; ++i) {
                    os << *i << ", ";
                }
               os << *rbegin();
            }
           os << ") override;" << endl;
            
        }
        void gen_cpp_delegate(const string& typeredirect, ostream& os) const {
            os << "  inline virtual void " << name << "(";
            int j=0;
            if (!empty()) {
                auto e=end();
                --e;
                for (auto i=cbegin(); i!=e; ++i) {
                    os << *i << " a" << j++ << ", ";
                }
               os << *rbegin() << " a" << j;
            }
           os << ") override { " << typeredirect << "::" << name << "(";
            j=0;
            if (!empty()) {
                auto e=end();
                --e;
                for (auto i=cbegin(); i!=e; ++i) {
                    os << "a" << j++ << ", ";
                }
                os << "a" << j;
            }
            os << "); }" << endl;
            
        }
    };


struct api_t: vector<f> {
    string src;
    string name;

    static void warn_h(ostream& os) {
        os << "//------------------generated by apitool- do not edit" << endl;
    }
    void info(ostream&os) const {
       os << "// " << name << " - master file: us/apitool/" << src <<  endl;
    }
    static void warn_f(ostream& os) {
       os << "//-/----------------generated by apitool- do not edit" << endl;
    }

    static api_t from_stream(istream&is) {
        api_t api;
        while (is.good()) {
            f i=f::from_stream(is);
            if (!is.good()) {
                break;
            }
            api.push_back(i);
        }
        return api;
    }

    static api_t load(string file) {
        string nm=file;
        file=string("data/")+file;
        ifstream is(file);
        auto a=from_stream(is);
        a.name=nm;
        a.src=file;
        return move(a);
    }
 

/*
        void gen_cpp_purevir(auto header, auto footer, ostream& os) const {
//            os << "//------------------generated by apitool- do not edit" << endl;
            header(os);
            for (auto&i:*this) {
                i.gen_cpp_purevir(os);
            }
            footer(os);
//            os << "//-/----------------generated by apitool- do not edit" << endl;
        }
*/

    void gen_cpp_override(ostream&os) const {
        info(os);
            for (auto&i:*this) {
                i.gen_cpp_override(os);
            }
    }
    void gen_cpp_purevir(ostream&os) const {
        info(os);
            for (auto&i:*this) {
                i.gen_cpp_purevir(os);
            }
    }
    void gen_cpp_wallet_daemon_delegate(const string& t, ostream&os) const {
        info(os);
            for (auto&i:*this) {
                i.gen_cpp_delegate(t,os);
            }
    }
/*
vector<string> f{ "wallet_base","balance_query","list_query",
    "new_address_query","add_address_query","tx_make_p2pkh_query",
    "tx_sign_query","tx_send_query","tx_decode_query",
    "tx_check_query","pair_query","unpair_query","list_devices_query",
    "ping","response" };
*/
    vector<string> get_protocol_vector() const {
        vector<string> r;
        r.push_back(name + "_base");
        for (auto&i:*this) {
             i.get_protocol_vector(name,r);
        }
        return r;
    }

        void gen_protocol_cpp_body(const vector<string>& v, int nbase, ostream& os) const {
            info(os);
            assert(!v.empty());
            int n=0;
            const string& base=v[0];
            os << "    static constexpr uint16_t " << v[n++] << "{" << nbase << "};" << endl;
            for (;n<v.size();++n) {
                os << "    static constexpr uint16_t " << v[n] << "{" << base << "+" << (n-1) << "};" << endl;
            }
        }
        void gen_gov_protocol_cpp_body(const vector<string>& v, int nbase, ostream& os) const {
            info(os);
            assert(!v.empty());
            int n=0;
            const string& base=v[0];
            os << "    static constexpr uint16_t " << v[n++] << "{" << nbase << "};" << endl;
            for (;n<v.size();++n) {
                os << "    static constexpr uint16_t " << v[n] << "{((" << base << "+" << n-1 << ")<<2)+protocol_suffix};" << endl;
            }
        }
        void gen_protocol_cpp(const vector<string>& v, int nbase, ostream& os) const {
            warn_h(os);
            gen_protocol_cpp_body(v,nbase,os);
            warn_f(os);
        }
        void gen_gov_protocol_cpp(const vector<string>& v, int nbase, ostream& os) const {
            warn_h(os);
            gen_gov_protocol_cpp_body(v,nbase,os);
            warn_f(os);
        }
        void gen_protocol_java_body(const vector<string>& v, int nbase, ostream& os) const {
            info(os);
            assert(!v.empty());
            int n=0;
            const string& base=v[0];
            os << "    public static final short protocol_" << v[n++] << " = " << nbase << ";" << endl;
            for (;n<v.size();++n) {
                os << "    public static final short protocol_" << v[n] << " = protocol_" << base << "+" << (n-1) << ";" << endl;
            }
        }
        void gen_gov_protocol_java_body(const vector<string>& v, int nbase, ostream& os) const {
            info(os);
            assert(!v.empty());
            int n=0;
            const string& base=v[0];
            os << "    public static final short protocol_" << v[n++] << " = " << nbase << ";" << endl;
            for (;n<v.size();++n) {
                os << "    public static final short protocol_" << v[n] << " = ((protocol_" << base << "+" << n-1 << ")<<2)+protocol_suffix;" << endl;
            }
        }
        void gen_protocol_java(const vector<string>& v, int nbase, ostream& os) const {
            warn_h(os);
            gen_protocol_java_body(v,nbase, os);
            warn_f(os);
        }
        void gen_gov_protocol_java(const vector<string>& v, int nbase, ostream& os) const {
            warn_h(os);
            gen_gov_protocol_java_body(v,nbase, os);
            warn_f(os);
        }



};



void gen_functions_cpp_override(const api_t&a) {
    ostringstream fn;
    fn << file_prefix << "functions_" << a.name << "_cpp_override";
    string file=fn.str();
    cout << "writting file " << file  << endl;
    ofstream os(file);
    a.warn_h(os);
    a.gen_cpp_override(os);
    a.warn_f(os);
}

void gen_functions_cpp_purevir(const api_t&a) {
    ostringstream fn;
    fn << file_prefix << "functions_" << a.name << "_cpp_purevir";
    string file=fn.str();
    cout << "writting file " << file  << endl;
    ofstream os(file);
    a.warn_h(os);
    a.gen_cpp_purevir(os);
    a.warn_f(os);
}

void gen_functions_wallet_daemon_impl(const api_t&w, const api_t&p) {
    ostringstream fn;
    fn << file_prefix << "functions_wallet-daemon" << "_cpp_impl";
    string file=fn.str();
    cout << "writting file " << file  << endl;
    ofstream os(file);
    w.warn_h(os);
    w.gen_cpp_wallet_daemon_delegate("w",os);
    os << endl;
    p.gen_cpp_wallet_daemon_delegate("p",os);
    w.warn_f(os);
}

void gen_protocol(const api_t& a, int base) {
    auto v=a.get_protocol_vector();
    {
    ostringstream fn;
    fn << file_prefix << "protocol_" << a.name << "_cpp";
    cout << "writting file " << fn.str()  << endl;
    ofstream os(fn.str());
    a.gen_protocol_cpp(v,base,os);
    }
    {
    ostringstream fn;
    fn << file_prefix << "protocol_" << a.name << "_java";
    cout << "writting file " << fn.str()  << endl;
    ofstream os(fn.str());
    a.gen_protocol_java(v,base,os);
    }

}

void gen_wallet_daemon_protocol(const api_t& w, const vector<string>& vw, int wbase, const api_t& p, const vector<string>& vp, int pbase) {
    {
    ostringstream fn;
    fn << file_prefix << "protocol_" << "wallet-daemon" << "_cpp";
    cout << "writting file " << fn.str()  << endl;
    ofstream os(fn.str());
    api_t::warn_h(os);
    w.gen_protocol_cpp_body(vw,wbase,os);
    p.gen_protocol_cpp_body(vp,pbase,os);
    api_t::warn_f(os);
    }
    {
    ostringstream fn;
    fn << file_prefix << "protocol_" << "wallet-daemon" << "_java";
    cout << "writting file " << fn.str()  << endl;
    ofstream os(fn.str());
    api_t::warn_h(os);
    w.gen_protocol_java_body(vw,wbase,os);
    p.gen_protocol_java_body(vp,pbase,os);
    api_t::warn_f(os);
    }
}

void gen_protocol(const api_t& a, const vector<string>& v, int base) {
    {
    ostringstream fn;
    fn << file_prefix << "protocol_" << a.name << "_cpp";
    cout << "writting file " << fn.str()  << endl;
    ofstream os(fn.str());
    a.gen_protocol_cpp(v,base,os);
    }
    {
    ostringstream fn;
    fn << file_prefix << "protocol_" << a.name << "_java";
    cout << "writting file " << fn.str()  << endl;
    ofstream os(fn.str());
    a.gen_protocol_java(v,base,os);
    }
}
void gen_gov_protocol(const api_t& a, const vector<string>& v, int base) {
    {
    ostringstream fn;
    fn << file_prefix << "protocol_" << a.name << "_cpp";
    cout << "writting file " << fn.str()  << endl;
    ofstream os(fn.str());
    a.gen_gov_protocol_cpp(v,base,os);
    }
    {
    ostringstream fn;
    fn << file_prefix << "protocol_" << a.name << "_java";
    cout << "writting file " << fn.str()  << endl;
    ofstream os(fn.str());
    a.gen_gov_protocol_java(v,base,os);
    }
}


void do_wallet_daemon(const api_t& w,const vector<string>& vw,int wbase,const api_t& p, const vector<string>& vp,int pbase) {
    gen_functions_wallet_daemon_impl(w,p);
    gen_wallet_daemon_protocol(w,vw,wbase,p,vp,pbase);
}

void do_api(const api_t& a, const vector<string>& v, int base) {
    gen_functions_cpp_purevir(a);
    gen_functions_cpp_override(a);
    gen_protocol(a,v,base);

}
void do_gov_api(const api_t& a,const vector<string>& v,int base) {
    gen_functions_cpp_purevir(a);
    gen_functions_cpp_override(a);
    gen_gov_protocol(a,v,base);
}

int main(int argc, char**argv) {
    auto w=api_t::load("wallet");
    auto p=api_t::load("pairing");
    auto gid=api_t::load("gov_id");

    int wbase=0;
    int pbase=100;
    int gidbase=200;

    auto vw=w.get_protocol_vector();
    auto vp=p.get_protocol_vector();
    auto vgid=gid.get_protocol_vector();

    do_api(w,vw,wbase);
    do_api(p,vp,pbase);
    do_wallet_daemon(w,vw,wbase,p,vp,pbase);

    do_gov_api(gid,vgid,gidbase);
}
