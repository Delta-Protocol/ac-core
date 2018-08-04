#include <iostream>
#include <vector>
#include <sstream>
#include <cassert>
#include <fstream>
#include <functional>
#include <unordered_map>
using namespace std;

string file_prefix="apitool_generated__";

struct langt {
    string java;
    string php;
};

unordered_map<string,langt> lang_type{
{"const string&",{"String","?"}},
{"const pub_t&",{"String","?"}},
{"const priv_t&",{"String","?"}},
{"bool",{"boolean","?"}},
{"const hash_t&",{"String","?"}},
{"const cash_t&",{"long","?"}},
{"const tx_make_p2pkh_input&",{"tx_make_p2pkh_input","?"}},
{"sigcode_t",{"sigcode_t","?"}},
{"ostream&",{"OutputStream","?"}},
};

const string& lookup_java(const string& cpp_type) {
    auto i=lang_type.find(cpp_type);
    if (i==lang_type.end()) {
        cerr << "Type " << cpp_type << " not found." << endl;
        exit(1);
    }
    return i->second.java;
}


void include_snippet(const string& file,ostream&os) {
    ifstream f(file);
    while(f.good()) {
        string line;
        getline(f,line);
        os << line << endl;
    }
}

    struct f: vector<string>  {
        typedef vector<string> args;
        string name;
        string fcgi;
        
        string service;
        string custom_rpc_impl;

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
            r.push_back("ostream&");

            getline(is,r.fcgi);
            getline(is,r.custom_rpc_impl);
            return r;
        }

        void compute_get_protocol_vector(const string& prefix, vector<string>& r) {
            ostringstream os;
            os << prefix << '_' << name;
            r.push_back(os.str());
            service=os.str();
        }

        void gen_fcgi_index_cpp(ostream& os) const {
            if (fcgi=="-") return;
            os << "os << url << \"/?<b>" << fcgi << "</b>\" << endl;" << endl;
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

        void gen_java_purevir(ostream& os) const {
            os << "  void " << name << "(";
            if (!empty()) {
                auto e=end();
                --e;
                int a=0;
                for (auto i=cbegin(); i!=e; ++i) {
                    ostringstream pname;
                    pname << "arg" << a++;
                    os << lookup_java(*i) << " " << pname.str() << ", ";
                }
               os << lookup_java(*rbegin()) << " os";
            }
           os << ");" << endl;
            
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

        void gen_cpp_rpc_impl(ostream&os) const {
            os << "void c::" << name << "(";
            int j=0;
            if (!empty()) {
                auto e=end();
                --e;
                for (auto i=cbegin(); i!=e; ++i) {
                    os << *i << " a" << j++ << ", ";
                }
               os << *rbegin() << " a" << j;
            }
           os << ") {" << endl;
            j=0;
            assert(!empty());
            if (size()==1) {
                   if (custom_rpc_impl=="-") {
                        os << "    ask(" << service << ", a0);" << endl;
                    }
                   else {
                       os << custom_rpc_impl << endl;
                   }
                    
            }
            else if (size()==2 && (*this)[0]=="const string&") {
                   if (custom_rpc_impl=="-") {
                        os << "    ask(" << service << ", a0, a1);" << endl;
                    }
                   else {
                       os << custom_rpc_impl << endl;
                   }
            }
            else {
                   os << "    ostringstream o; o << ";
                   auto e=end();
                   --e; --e;
                   for (auto i=cbegin(); i!=e; ++i) {
                       os << "a" << j++ << " << ' ' << ";
                   }
                   os << "a" << j++;
                   os << ";" << endl;
                   if (custom_rpc_impl=="-") {
                       os << "    ask(" << service << ", o.str(), a" << j << ");" << endl;
                   }
                   else {
                       os << custom_rpc_impl << endl;
                   }
            }
            os << "}" << endl;
        }

        void gen_java_rpc_impl(ostream&os) const {
            os << "@Override public void " << name << "(";
            if (!empty()) {
                auto e=end();
                --e;
                int a=0;
                for (auto i=cbegin(); i!=e; ++i) {
                    ostringstream args;
                    args << "a" << a++;
                    os << lookup_java(*i) << " " << args.str() << ", ";
                }
               os << lookup_java(*rbegin()) << " a" << a;
            }
            os << ") {" << endl;
            int j=0;
            assert(!empty());
            if (size()==1) {
                   if (custom_rpc_impl=="-") {
                        os << "    endpoint.ask(protocol." << service << ", a0);" << endl;
                    }
                   else {
                       os << custom_rpc_impl << endl;
                   }
                    
            }
            else if (size()==2 && lookup_java((*this)[0])=="String") {
                   if (custom_rpc_impl=="-") {
                        os << "    endpoint.ask(protocol." << service << ", a0, a1);" << endl;
                    }
                   else {
                       os << custom_rpc_impl << endl;
                   }
            }
            else {
                   os << "    String o=new String();" << endl;                    
                   auto e=end();
                   --e; --e;
                   auto i=cbegin(); 
                   for (; i!=e; ++i) {
                     if (lookup_java(*i)=="String") {
                        os << "    o += a" << j++ << " + ' ';" << endl;
                     }
                     else {
                        os << "    o += to_string(a" << j++ << ") + ' ';" << endl;
                     }
                   }
                   if (lookup_java(*i)=="String") {
                       os << "    o += a" << j++ << ";" << endl;
                   }
                   else {
                        os << "    o += to_string(a" << j++ << ") + ' ';" << endl;
                   }
                   if (custom_rpc_impl=="-") {
                       os << "    endpoint.ask(protocol." << service << ", o, a" << j << ");" << endl;
                   }
                   else {
                       os << custom_rpc_impl << endl;
                   }
            }
            os << "}" << endl;
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

        void gen_java_delegate(const string& typeredirect, ostream& os) const {
            os << "  @Override public void " << name << "(";
            int j=0;
            if (!empty()) {
                auto e=end();
                --e;
                for (auto i=cbegin(); i!=e; ++i) {
                    os << lookup_java(*i) << " a" << j++ << ", ";
                }
               os << lookup_java(*rbegin()) << " a" << j;
            }
           os << ") { " << typeredirect << "." << name << "(";
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

        void gen_cpp_service_router(ostream&os) const {
            os << "    case " << service << ": { return send_response__" << service << "(c,d); }" << endl;
        }
        void gen_cpp_service_handlers(ostream&os) const {
            os << "bool c::send_response__" << service << "(socket::peer_t* c, socket::datagram* d) {" << endl;
            os << "   abort();" << endl;
            os << "}" << endl;
        }
        void gen_cpp_service_handler_headers(ostream&os) const {
            os << "bool send_response__" << service << "(socket::peer_t*, socket::datagram*);" << endl;
        }
    };

struct api_t: vector<f> {
    string src;
    string name;

    vector<string> v;

        static string protocol_prefix_cpp;
        static string protocol_prefix_java;

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
        a.v=a.compute_get_protocol_vector();
        return move(a);
    }
 
    void gen_fcgi_index_cpp(ostream&os) const {
        info(os);
            for (auto&i:*this) {
                i.gen_fcgi_index_cpp(os);
            }
    }

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
    void gen_java_purevir(ostream&os) const {
        info(os);
            for (auto&i:*this) {
                i.gen_java_purevir(os);
            }
    }
    void gen_cpp_wallet_daemon_delegate(const string& t, ostream&os) const {
        info(os);
            for (auto&i:*this) {
                i.gen_cpp_delegate(t,os);
            }
    }
    void gen_java_wallet_daemon_delegate(const string& t, ostream&os) const {
        info(os);
            for (auto&i:*this) {
                i.gen_java_delegate(t,os);
            }
    }

    vector<string> compute_get_protocol_vector() {
        vector<string> r;
        r.push_back(name + "_base");
        for (auto&i:*this) {
             i.compute_get_protocol_vector(name,r);
        }
        return r;
    }
        
        void gen_cpp_rpc_impl(ostream&os) const {
            for (auto&i:*this) {
                i.gen_cpp_rpc_impl(os);
                os << endl;
            }

        }

        void gen_java_rpc_impl(ostream&os) const {
            for (auto&i:*this) {
                i.gen_java_rpc_impl(os);
                os << endl;
            }

        }

        void gen_cpp_service_router(ostream&os) const {
            info(os);
            for (auto&i:*this) {
                i.gen_cpp_service_router(os);
            }
        }
        void gen_cpp_service_handlers(ostream&os) const {
            info(os);
            for (auto&i:*this) {
                i.gen_cpp_service_handlers(os);
                os << endl;
            }
        }
        void gen_cpp_service_handler_headers(ostream&os) const {
            info(os);
            for (auto&i:*this) {
                i.gen_cpp_service_handler_headers(os);
            }
        }


        void gen_protocol_cpp_body(int nbase, ostream& os) const {
            info(os);
            assert(!v.empty());
            int n=0;
            const string& base=v[0];
            os << "    static constexpr uint16_t " << v[n++] << "{" << nbase << "};" << endl;
            int fid=0;
            for (;n<v.size();++n,fid+=2) {
                os << "    static constexpr uint16_t " << v[n] << "{" << base << "+" << fid << "};" << endl;
            }
        }

        void gen_gov_protocol_cpp_body(int nbase, ostream& os) const {
            info(os);
            assert(!v.empty());
            int n=0;
            const string& base=v[0];
            os << "    static constexpr uint16_t " << protocol_prefix_cpp << "" << v[n++] << "{" << nbase << "};" << endl;
            for (;n<v.size();++n) {
                os << "    static constexpr uint16_t " << protocol_prefix_cpp << "" << v[n] << "{((" << protocol_prefix_cpp << base << "+" << (n-1) << ")<<2)+protocol_suffix};" << endl;
            }
        }
        void gen_protocol_cpp(int nbase, ostream& os) const {
            warn_h(os);
            gen_protocol_cpp_body(nbase,os);
            warn_f(os);
        }
        void gen_gov_protocol_cpp(int nbase, ostream& os) const {
            warn_h(os);
            gen_gov_protocol_cpp_body(nbase,os);
            warn_f(os);
        }
        void gen_protocol_java_body(int nbase, ostream& os) const {
            info(os);
            assert(!v.empty());
            int n=0;
            const string& base=v[0];
            os << "    public static final short " << protocol_prefix_java << v[n++] << " = " << nbase << ";" << endl;
            int fid=0;
            for (;n<v.size();++n, fid+=2) {
                os << "    public static final short " << protocol_prefix_java << v[n] << " = " << protocol_prefix_java << base << "+" << fid << ";" << endl;
            }
        }
        void gen_gov_protocol_java_body(int nbase, ostream& os) const {
            info(os);
            assert(!v.empty());
            int n=0;
            const string& base=v[0];
            os << "    public static final short " << protocol_prefix_java << v[n++] << " = " << nbase << ";" << endl;
            for (;n<v.size();++n) {
                os << "    public static final short " << protocol_prefix_java << v[n] << " = ((" << protocol_prefix_java << base << "+" << n-1 << ")<<2)+protocol_suffix;" << endl;
            }
        }
        void gen_protocol_java(int nbase, ostream& os) const {
            warn_h(os);
            gen_protocol_java_body(nbase, os);
            warn_f(os);
        }
        void gen_gov_protocol_java(int nbase, ostream& os) const {
            warn_h(os);
            gen_gov_protocol_java_body(nbase, os);
            warn_f(os);
        }
};

string api_t::protocol_prefix_cpp{""};
//string api_t::protocol_prefix_java{"protocol_"};
string api_t::protocol_prefix_java{""};

void gen_cpp_service_router(const api_t&a) {
    ostringstream fn;
    fn << file_prefix << "protocol_" << a.name << "_cpp_service_router";
    string file=fn.str();
    cout << "writting file " << file  << endl;
    ofstream os(file);
    a.warn_h(os);
    a.gen_cpp_service_router(os);
    a.warn_f(os);
}
void gen_cpp_service_handlers(const api_t&a) {
    ostringstream fn;
    fn << file_prefix << "protocol_" << a.name << "_cpp_service_handlers";
    string file=fn.str();
    cout << "writting file " << file  << endl;
    ofstream os(file);
    a.warn_h(os);
    a.gen_cpp_service_handlers(os);
    a.warn_f(os);
}
void gen_cpp_service_handler_headers(const api_t&a) {
    ostringstream fn;
    fn << file_prefix << "protocol_" << a.name << "_cpp_service_handler_headers";
    string file=fn.str();
    cout << "writting file " << file  << endl;
    ofstream os(file);
    a.warn_h(os);
    a.gen_cpp_service_handler_headers(os);
    a.warn_f(os);
}

void gen_functions_cpp_rpc_impl(const api_t&a) {
    ostringstream fn;
    fn << file_prefix << "functions_" << a.name << "_cpp_rpc-impl";
    string file=fn.str();
    cout << "writting file " << file  << endl;
    ofstream os(file);
    a.warn_h(os);
    a.gen_cpp_rpc_impl(os);
    a.warn_f(os);
}

void gen_functions_java_rpc_impl(const api_t&a) {
    ostringstream fn;
    fn << file_prefix << "functions_" << a.name << "_java_rpc-impl";
    string file=fn.str();
    cout << "writting file " << file  << endl;
    ofstream os(file);
    a.warn_h(os);
    a.gen_java_rpc_impl(os);
    a.warn_f(os);
}

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

void gen_functions_java_purevir(const api_t&a) {
    ostringstream fn;
    fn << file_prefix << "functions_" << a.name << "_java_purevir";
    string file=fn.str();
    cout << "writting file " << file  << endl;
    ofstream os(file);
    a.warn_h(os);
    a.gen_java_purevir(os);
    a.warn_f(os);
}

void gen_fcgi_index_cpp(const api_t&a) {
    ostringstream fn;
    fn << file_prefix << "links_" << a.name << "_fcgi_query_strings";
    string file=fn.str();
    cout << "writting file " << file  << endl;
    ofstream os(file);
    a.warn_h(os);
    a.gen_fcgi_index_cpp(os);
    a.warn_f(os);
}

void gen_functions_wallet_daemon_cpp_impl(const api_t&w, const api_t&p) {
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

void gen_functions_wallet_daemon_java_impl(const api_t&w, const api_t&p) {
    ostringstream fn;
    fn << file_prefix << "functions_wallet-daemon" << "_java_impl";
    string file=fn.str();
    cout << "writting file " << file  << endl;
    ofstream os(file);
    w.warn_h(os);
    w.gen_java_wallet_daemon_delegate("w",os);
    os << endl;
    p.gen_java_wallet_daemon_delegate("p",os);
    w.warn_f(os);
}

void gen_protocol(const api_t& a, int base) {
    {
    ostringstream fn;
    fn << file_prefix << "protocol_" << a.name << "_cpp";
    cout << "writting file " << fn.str()  << endl;
    ofstream os(fn.str());
    a.gen_protocol_cpp(base,os);
    }
    {
    ostringstream fn;
    fn << file_prefix << "protocol_" << a.name << "_java";
    cout << "writting file " << fn.str()  << endl;
    ofstream os(fn.str());
    a.gen_protocol_java(base,os);
    }

}
void gen_cpp_wallet_daemon_service_router(const api_t& w, const api_t& p) {
    ostringstream fn;
    fn << file_prefix << "protocol_" << "wallet-daemon" << "_cpp_service_router";
    string file=fn.str();
    cout << "writting file " << file  << endl;
    ofstream os(file);
    api_t::warn_h(os);
    w.gen_cpp_service_router(os);
    p.gen_cpp_service_router(os);
    api_t::warn_f(os);
}

void gen_cpp_wallet_daemon_service_handlers(const api_t& w, const api_t& p) {
    ostringstream fn;
    fn << file_prefix << "protocol_" << "wallet-daemon" << "_cpp_service_handlers";
    string file=fn.str();
    cout << "writting file " << file  << endl;
    ofstream os(file);
    api_t::warn_h(os);
    w.gen_cpp_service_handlers(os);
    p.gen_cpp_service_handlers(os);
    api_t::warn_f(os);
}

void gen_cpp_wallet_daemon_service_handler_headers(const api_t& w, const api_t& p) {
    ostringstream fn;
    fn << file_prefix << "protocol_" << "wallet-daemon" << "_cpp_service_handler_headers";
    string file=fn.str();
    cout << "writting file " << file  << endl;
    ofstream os(file);
    api_t::warn_h(os);
    w.gen_cpp_service_handler_headers(os);
    p.gen_cpp_service_handler_headers(os);
    api_t::warn_f(os);
}


void gen_wallet_daemon_protocol(const api_t& w, int wbase, const api_t& p, int pbase) {
    {
    ostringstream fn;
    fn << file_prefix << "protocol_" << "wallet-daemon" << "_cpp";
    cout << "writting file " << fn.str()  << endl;
    ofstream os(fn.str());
    api_t::warn_h(os);
    w.gen_protocol_cpp_body(wbase,os);
    p.gen_protocol_cpp_body(pbase,os);
    api_t::warn_f(os);
    }
    {
    ostringstream fn;
    fn << file_prefix << "protocol_" << "wallet-daemon" << "_java";
    cout << "writting file " << fn.str()  << endl;
    ofstream os(fn.str());
    api_t::warn_h(os);
    w.gen_protocol_java_body(wbase,os);
    p.gen_protocol_java_body(pbase,os);
    api_t::warn_f(os);
    }
}

void gen_protocol(const api_t& a, const vector<string>& v, int base) {
    {
    ostringstream fn;
    fn << file_prefix << "protocol_" << a.name << "_cpp";
    cout << "writting file " << fn.str()  << endl;
    ofstream os(fn.str());
    a.gen_protocol_cpp(base,os);
    }
    {
    ostringstream fn;
    fn << file_prefix << "protocol_" << a.name << "_java";
    cout << "writting file " << fn.str()  << endl;
    ofstream os(fn.str());
    a.gen_protocol_java(base,os);
    }
}
void gen_gov_protocol(const api_t& a, int base) {
    {
    ostringstream fn;
    fn << file_prefix << "protocol_" << a.name << "_cpp";
    cout << "writting file " << fn.str()  << endl;
    ofstream os(fn.str());
    a.gen_gov_protocol_cpp(base,os);
    }
    {
    ostringstream fn;
    fn << file_prefix << "protocol_" << a.name << "_java";
    cout << "writting file " << fn.str()  << endl;
    ofstream os(fn.str());
    a.gen_gov_protocol_java(base,os);
    }
}

void do_wallet_daemon(const api_t& w, int wbase, const api_t& p, int pbase) {
    gen_functions_wallet_daemon_cpp_impl(w,p);
    gen_functions_wallet_daemon_java_impl(w,p);
    gen_wallet_daemon_protocol(w,wbase,p,pbase);
//    gen_functions_cpp_rpc_impl(w,p);
    gen_cpp_wallet_daemon_service_router(w,p);
    gen_cpp_wallet_daemon_service_handlers(w,p);
    gen_cpp_wallet_daemon_service_handler_headers(w,p);

}

void do_common_api(const api_t& a, int base) {
    gen_functions_cpp_purevir(a);
    gen_functions_java_purevir(a);
    gen_functions_cpp_override(a);
    gen_functions_cpp_rpc_impl(a);
    gen_functions_java_rpc_impl(a);
    gen_cpp_service_router(a);
    gen_cpp_service_handlers(a);
    gen_cpp_service_handler_headers(a);
}

void do_api(const api_t& a, int base) { //wallet
    do_common_api(a,base);
    gen_fcgi_index_cpp(a);
    gen_protocol(a,base);

}

void do_gov_api(const api_t& a,int base) { //gov
    do_common_api(a,base);
    gen_gov_protocol(a,base);
}

int main(int argc, char**argv) {
    auto w=api_t::load("wallet");
    auto p=api_t::load("pairing");
    auto gid=api_t::load("gov_id");
    auto gsocket=api_t::load("gov_socket");

    int wbase=0;
    int pbase=100;

    int gsocketbase=100;
    int gidbase=200;

    do_api(w,wbase);
    do_api(p,pbase);
    do_wallet_daemon(w,wbase,p,pbase);
    do_gov_api(gsocket,gsocketbase);
    do_gov_api(gid,gidbase);
}
