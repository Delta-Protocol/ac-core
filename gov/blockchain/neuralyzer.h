
#ifndef USGOV_a496dccebf355d2ca569d753ff4e5ffd1ecc04315f8273d302ba02d04a1d9069
#define USGOV_a496dccebf355d2ca569d753ff4e5ffd1ecc04315f8273d302ba02d04a1d9069

namespace us{ namespace gov {
using namespace std;

struct neuralyzer {
    neuralyzer(): root(0) {
    }
    ~neuralyzer() {
        delete head;
    }

    struct link {
        link(const hast_t& block, link* parent):parent(parent), block(block) {
        }
        ~link() {
            delete parent;
        }
        hash_t block;
        link*parent;
    };
    link* head;
    
    void push(const hash_t& block) {
        head=new link(block,head);                
    }

  
};

}}

#endif



