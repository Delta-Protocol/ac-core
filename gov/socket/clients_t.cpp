/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#include "clients_t.h"

#include <unistd.h>
#include <algorithm>
#include <atomic>
#include <cassert>
#include <utility>

#include "us/gov/likely.h"

using namespace std;
using namespace us::gov::socket;

void clients_t::add(client* c, bool wakeupselect) {
    m_wadd.add(c);
    if (wakeupselect) read_sockets();
}

void clients_t::remove(client* c) {
    if (m_wremove.add(c))
        read_sockets();
}

void clients_t::read_sockets() {
    static char w='w';
    ::write(m_locli.get_sock(), &w, 1);
}

void clients_t::grow() {
    unordered_set<client*> t;
    {
        lock_guard<mutex> lock(m_wadd.m_mx);
        t=m_wadd;
        m_wadd.clear();
    }
    {
        lock_guard<mutex> lock(m_mx);
        for (auto i:t) {
            assert(i);
            assert(find(i->get_sock())==end());
            assert(i->get_sock()!=0);
            emplace(i->get_sock(),i);
        }
    }
    if (!t.empty()) read_sockets();
}

void clients_t::shrink() {
    unordered_set<client*> copy;
    {
        lock_guard<mutex> lock(m_wremove.m_mx);
        copy=m_wremove;
        m_wremove.clear();
    }

    lock_guard<mutex> lock(m_mx);
    for (auto c:copy) {
        iterator i;
        c->on_detach();
        i=find(c->get_sock());
        if (likely(i!=end())) { //it is not in main container
            erase(i); //no other thread can remove from main container
            m_attic.add(c);
        }
        else {
            if (m_wadd.remove(c)) {
                m_attic.add(c);
            }
        }
    }
}

clients_t::~clients_t() {
    {
        lock_guard<mutex> lock(m_mx);
        for (auto i:*this) m_wremove.add(i.second); // flush me
    }

    shrink(); // flush wremove
}

vector<int> clients_t::update() {
    shrink();
    grow();
    m_attic.purge();

    vector<int> s;
    vector<client*> a;

    {
        lock_guard<mutex> lock(m_mx);
        s.reserve(size());
        a.reserve(size());
        for (auto i=begin(); i!=end();) {
            auto& c=*i->second;
            if (likely(c.get_sock()) ) { //disconnected client?
                if (!c.load_busy()) 
                    s.emplace_back(c.get_sock());
                a.emplace_back(&c);
                ++i;
            }
            else {
                c.on_detach();
                m_attic.add(i->second);
                i=erase(i);
            }
        }
    }

    {
        lock_guard<mutex> lock(m_mx_active);
        m_active=a;
    }

    return move(s);
}

clients_t::attic_t::~attic_t() {
    for (auto&i:*this) delete i;
}

void clients_t::attic_t::add(client*cl) {
    emplace(cl);
}

void clients_t::attic_t::purge() {
    for (auto i=begin(); i!=end(); ) {
        if ((*i)->load_busy()) {
            ++i;
            continue;
        }
        else {
            delete *i;
            i=erase(i);

        }
    }
}

clients_t::rmlist::~rmlist() {
    lock_guard<mutex> lock(m_mx);
    assert(empty());
}

bool clients_t::rmlist::add(client* c) {
    if(c->get_sock()==0) 
        return false;
    lock_guard<mutex> lock(m_mx);
    emplace(c);
    return true;
}

void clients_t::rmlist::dump(ostream& os) const {
    lock_guard<mutex> lock(m_mx);
    os << "Size: " << size() << endl;
    for (auto i:*this) {
        i->dump_all(os);
        os << endl;
    }
}

clients_t::wait::~wait() {
    lock_guard<mutex> lock(m_mx);
    for (auto i:*this) delete i;
}

void clients_t::wait::add(client* c) {
    lock_guard<mutex> lock(m_mx);
    emplace(c);
}

bool clients_t::wait::remove(client* c) { //dont delete
    lock_guard<mutex> lock(m_mx);
    iterator i=unordered_set<client*>::find(c);
    if (i==end()) return false;
    erase(i);
    return true;
}

bool clients_t::wait::find(const client& c) const {
    lock_guard<mutex> lock(m_mx);
    return unordered_set<client*>::find(const_cast<client*>(&c))!=end();
}

void clients_t::wait::dump(ostream& os) const {
    lock_guard<mutex> lock(m_mx);
    os << "Size: " << size() << endl;
    for (auto i:*this) {
        i->dump_all(os);
        os << endl;
    }
}

vector<client*> clients_t::active() const {
    lock_guard<mutex> lock(m_mx_active);
    return m_active;
}

void clients_t::dump(ostream& os) const {
    int n=0;
    for (auto& i:active()) {
        os << "client #" << n++ << ": "; i->dump(os); os << endl;
    }

    os << "Total active: " << n << endl;
    os << "*add buffer: " << endl;
    m_wadd.dump(os);
    os << "*remove buffer: " << endl;
    m_wremove.dump(os);
}
