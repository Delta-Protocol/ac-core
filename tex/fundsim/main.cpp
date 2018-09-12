#include <iostream>
#include <chrono>
#include <cmath>


using namespace std;

struct params {
	
	
};

struct user {
	double trade_rate{0}; //once a day
	double trade_amount{0};

	double daily_profit(const double& commtrade) const {
		return trade_amount*trade_rate+commtrade;
	}
};

struct webuser: user {
	webuser() {
	trade_rate=1; //once a day
	trade_amount=100;
	}
};
struct apiuser: user {
	apiuser() {
	trade_rate=20; //once a day
	trade_amount=50;
	}
};

//sell=1.36407
//buy=1.36428

using t_units=std::chrono::minutes;

struct spread {
	chrono::steady_clock::time_point t;
	double bid;
	double ask;

	int t_since(const chrono::steady_clock::time_point t0) const {
		return std::chrono::duration_cast<t_units>(t - t0).count();
	}
	void dump(std::ostream& os, const chrono::steady_clock::time_point& start) const {
		os <<  std::chrono::duration_cast<t_units>(t - start).count() << " " << ask << endl;
	}
};


#include <cex/engine.h>
#include <ctime>


struct forex: public vector<spread> {
	double incr=0;
	double prevbid=0;
	forex() {
		wcout << L"Loading fx" << endl;
		reserve(1000000);
		ifstream is("/home/marcos/fx/DAT_NT_EURGBP_T_LAST_201501.csv");
		while(!is.eof()) {
			string line;
			getline(is,line);
			istringstream ils(line);
			vector<string> tokens;
			string l;
			while(getline(ils,l,';')) {
				tokens.push_back(l);
			}
			if (tokens.size()!=3) continue;
			spread sp;
			std::tm tm;
			strptime(tokens[0].c_str(), "%Y%m%d %H%M%S", &tm);
			sp.t=std::chrono::steady_clock::from_time_t(std::mktime(&tm));
			sp.bid=1/stod(tokens[1]);
			if (abs(prevbid-sp.bid)>0.00004) {
				incr+=rand()%2==0?1.0:-1.0;
				prevbid=sp.bid;
			}
			sp.ask=sp.bid-0.00005-0.00005*(rand()%2==0?1.0:0.0);
			emplace_back(sp);
		}
		//load
		i=begin();
	}
	const spread& get_spread(int t) const {
//wcout << L"spread for t=" << t << endl;
		while (i!=end() && i->t_since(begin()->t)<t) {
//			wcout << L" *" << endl;
			++i;
		}
		if (i==end()) {
//			wcout << L"end" << endl;
			return *rbegin();
		}
//		wcout << L"noend " << endl;
		return *i;
	}
	void dump(const spread& item, std::ostream& os) const {
		item.dump(os,begin()->t);
	}
	void plot(std::ostream& os) const {
		for (auto& s:*this) {
			os << s.t.time_since_epoch().count() << " " << s.bid << endl;
		}

	}

	mutable iterator i;
};
using namespace curex::cex;
typedef curex::cex::id id;
typedef curex::cex::efp efp;
typedef curex::cex::market::spots spots;
typedef curex::cex::engine::tradeinput tradeinput;

struct omgr: curex::cex::engine::dorders {
	typedef map<id,double> orders;
	typedef map<efp,orders> queues;
	typedef curex::cex::string string;
	typedef curex::cex::ostream ostream;
	queues q;
	omgr(curex::cex::engine::dorders& b, int cid):curex::cex::engine::dorders(b) {
		for (auto& d:*this) {
			if (d.get_cid()!=cid) continue;
			if (d.key!=L'U') continue;
			auto i=q.find(d.get_encoded_rate());
			if (i==q.end()) {
				orders ords;
				ords.emplace(d._id,d.get_amount());
				q.emplace(d.get_encoded_rate(),move(ords));
				continue;
			}
			i->second.emplace(d._id,d.get_amount());
		}
	}
	void add(curex::cex::id id, auto rate, double amount) {
			//wcout << L"Adding " << id << L" " << rate << L" " << amount << endl;

			auto i=q.find(rate);
			if (i==q.end()) {
				orders ords;
				ords.emplace(id,amount);
				q.emplace(rate,move(ords));
				return;
			}
			i->second.emplace(id,amount);
	}
	void dump(curex::cex::ostream& os) const {
		for (auto ords:q) {
			wcout << ords.first << L" " << ords.second.size() << endl;
		}
	}
	bool exist_order(const efp& rate) const {
		auto i=q.find(rate);
		if (i==q.end()) return false;
		return !i->second.empty();
	}
	double volume(const efp& rate) const {
//	dump(wcout);
		auto i=q.find(rate);
		if (i==q.end()) return 0;
		double ans=0;
		for (auto& j:i->second) {
			ans+=j.second;
		}
		return ans;
	}
	double available() const {
		double ans=0;
		for (auto& i:q) {
		for (auto& j:i.second) {
			ans+=j.second;
		}
		}
		return ans;
	}
	void cancel_queue(engine& e, id acid, const string& pwd, auto rate) {
		ostream nullos(0);
		//wcout << L"Cancelling queue " << rate << endl;
		auto i=q.find(rate);
		if (i==q.end()) return;
		auto j=i->second.rbegin();
		while (j!=i->second.rend()) {
			e.cancel_order(acid,pwd,j->first,nullos);
			i->second.erase((++j).base());
			j=i->second.rbegin();
		}
		q.erase(i);
	}
	//returns amount not cancelled
	double cancel_amount(engine& e, id acid, const string& pwd, auto rate, double amount) {
		//wcout << L"Cancelling amount " << rate << L" " << amount << endl;
		ostream nullos(0);
		auto i=q.find(rate);
		if (i==q.end()) {
			//dump(wcout);
			//wcout << L"queue not found!! " << rate << endl;
			//abort();				
			return amount;
		}
		auto j=i->second.rbegin();
		while (j!=i->second.rend()) {
			e.cancel_order(acid,pwd,j->first,nullos);
			amount-=j->second;
			i->second.erase((++j).base());
			if (amount<0.01) break;
			j=i->second.rbegin();
		}
		if (i->second.empty()) q.erase(i);
		return amount;
	}

};

struct plan: vector<pair<curex::cex::efp,double>> {
	typedef curex::cex::string string;
	typedef curex::cex::ostream ostream;

	enum mode {
		bid, ask
	};


	plan(int cid, omgr& om, mode mode_):_mode(mode_), cid(cid), om(om) {
	}
	omgr& om;
	bool exists(const curex::cex::efp& rate) const {
		for (auto i:*this)
			if (i.first==rate) return true;
		return false;
	}		
	void execute(engine&e, id acid, const string& pwd, int cid, const string& mkt, bool low) { //"AUD/GBP"

		vector<curex::cex::efp> queues_to_cancel;
		ostream nullos(0);

		for (const auto& pi:om.q) { //cancel all orders not in plan
			auto rate=pi.first;
			if (exists(rate)) continue;
			queues_to_cancel.push_back(rate);
		}			
		for (auto i:queues_to_cancel) {
			om.cancel_queue(e,acid,pwd,i);
		}

		std::vector<tradeinput> input;
		for (const auto& pi:*this) { //from front to deep
			auto rate=pi.first;
			double expected_vol=pi.second;
		//om.dump(wcout);
			double cur_vol=om.volume(rate);
			//wcout << rate << L" expected " << expected_vol << L" cur " << cur_vol << endl;
			double diff=expected_vol-cur_vol;
			//wcout << L"diff " << diff << endl;
			while (diff<-1e-5) {
				om.cancel_amount(e,acid,pwd,rate,-diff);
				cur_vol=om.volume(rate);
				//wcout << rate << L" expected " << expected_vol << L" cur " << cur_vol << endl;
				diff=expected_vol-cur_vol;
				//wcout << L"diff " << diff << endl;
			}
			double wallet_funds=e.get_wallet(acid,pwd,cid,nullos);
			//wcout << rate << L" expected " << expected_vol << L" cur " << cur_vol << L" wallet " << wallet_funds << endl;
			if (diff>wallet_funds) {
				//wcout << L"fund wallet " << diff << L" " << wallet_funds << endl;
				if (!fund_wallet(e,acid,pwd,om,diff-wallet_funds,cid,rate)) {} //abort();
				wallet_funds=e.get_wallet(acid,pwd,cid,nullos);
				//wcout << L" wallet " << wallet_funds << endl;
				if(diff>wallet_funds) {
					//wcout << L" wallet " << wallet_funds << endl;
					diff=wallet_funds;
				}
			}
			if (diff>1e-5) {
				tradeinput ti;
				ti.code=L"S";
				ti.set_amount(diff);
				ti.set_cur(cid);
				ti.set_rate(rate);
				ti.rateunits=mkt;
				input.push_back(move(ti));	
			}
		}
		curex::cex::ostringstream os;
		if (low)
			e.trade_low(acid, pwd, input, os);
		else
			e.trade(acid, pwd, input, os);

		if (low) {
			curex::cex::istringstream ist(os.str());
			while(!ist.eof()) {
				curex::cex::string line;
			
		//wcout << os.str() << endl;
				getline(ist, line);					//U 1 S 500.00000 EUR 1.28755 EUR/GBP
				curex::cex::istringstream is(line);
				//wcout << "last line " << line << endl;
				curex::cex::string code;
				curex::cex::id id;
				curex::cex::string op;
				curex::cex::string am;
				is >> code;
				if(!code.empty()) {
					is >> id;
					is >> op;
					is >> am;
					if(code!=L"U") {
						wcout << os.str() << endl;
						abort();
					}

					assert(op==L"S");
					curex::cex::string cur;
					is >> cur;
					curex::cex::string srate;
					is >> srate;

		//wcout << am << L" " << curex::cex::decode(curex::cex::encode(diff)) << endl;
					//assert(am==curex::cex::decode(curex::cex::encode(diff)));
					om.add(id,curex::cex::encode(srate),curex::cex::decode_dbl(curex::cex::encode(am)));
			
				}
			}
		}

	}
	mode _mode;	
	int cid;	
	//const spots& _spots;
	bool fund_wallet(engine&e,id acid, const string& pwd,omgr& om,double amount,int cid,auto limitrate) {
		//wcout << L"fund_wallet " << cid << L" " << amount << L" " << limitrate << endl;
		for (auto i=rbegin(); i!=rend(); ++i) {
			auto rate=i->first;
		//wcout << L"check rate " << rate << endl;
			if (_mode==ask) {
				if (rate>=limitrate) {
		//wcout << L"limit reached(bid) " << rate << " " << limitrate << endl;
					return false;
				}
			}
			else {
				if (rate<=limitrate) {
		//wcout << L"limit reached(ask) " << rate << " " << limitrate << endl;
					return false;
				}
			}
			amount=om.cancel_amount(e,acid,pwd,rate,amount); //returns amount left
			if (amount<0.1) {
				//wcout << L"left amount to fund " << amount << endl;
				return true;
			}
		}
		wcout << L"Should not be here " << size() << endl;
		return false;
	}
	//const curex::cex::market::spots& _vr;
	void dump(const curex::cex::string& prefix, curex::cex::ostream& os) const {
		for (auto i:*this) {
			os << prefix << (_mode==ask?L"ask ":L"bid ") << curex::cex::decode(i.first) << L" " << i.second << endl;
		}
	}
};


struct manager {
	typedef curex::cex::string string;
	typedef curex::cex::ostream ostream;
	manager(string name, engine& e, int cur1, double amount1, int cur2, double amount2, double commdiscount): _e(e), _cur1(cur1), _cur2(cur2), name(name) {
		id nid{0};
		ostream nullos(0);
		_acpwd=L"8888888888";
		_acid=_e.new_account(_acpwd,nullos);
		_e.set_commission_discount(_acid,commdiscount);
		_e.delete_all_wallets(_acid,_acpwd,nullos);
		_e.deposit(_acid,_acpwd,cur1,amount1,nullos);
		_e.deposit(_acid,_acpwd,cur2,amount2,nullos);
		wcout << name << " acc id:" << _acid << endl;

	}
	manager(manager&&)=delete;
	manager(const manager&)=delete;
	~manager() {
		ostream nullos(0);
		_e.cancel_all_orders(_acid,_acpwd,nullos);
		wcout << "goodbye " << name << endl;
		_e.list_account(_acid,_acpwd,wcout);
	}


	string name;

	int _cur1, _cur2;
	engine& _e;
	id _acid;
	string _acpwd;
};


struct investor_manager: manager {
	investor_manager(engine& e, int cur1, double amount1, int cur2, double amount2): manager(L"investor", e, cur1, amount1, cur2, amount2, 1) {  //100% discount for investor
	}

	struct myplan: plan {
		myplan(const auto& vr, omgr& orders, int cid, double vol0_wallet, const auto& fx, mode mode_, int aperture_ticks):plan(cid, orders, mode_) {
			double vol0_ords=orders.available();
			double vol0=vol0_wallet+vol0_ords;
			//wcout << L"$ avail wallet:" << vol0_wallet << L" orders:" << vol0_ords << L" total:" << vol0 << endl;
			//bid: front es el rate mas pequenio 
			//ask: front es el rate mas grande
			auto spot=mode_==ask?vr.lower_rate(fx.ask):vr.upper_rate(fx.bid);

//wcout << "spot0=" << *spot;
			//for (int i=0; i<aperture_ticks; ++i)
			mode_==ask?spot-=aperture_ticks:spot+=aperture_ticks;
//wcout << " spot1=" << *spot << L" ask " << (mode_==ask) << endl;
			auto dist=distance(vr.begin(),spot);
			if (dist<0 || dist >=vr.size() ) {
				return;
			}

//wcout << *vr.upper_rate(fx.ask) <<  L" " << fx.ask << L" " << fx.bid << L" " << *vr.lower_rate(fx.bid) << endl;
			double pvol=0.001;
			double vol=vol0;
			while(spot!=vr.end() && vol>0) {
				double am=vol0*pvol;
				if (am>vol) am=vol;
				vol-=am;
				emplace_back(pair<curex::cex::efp,double>(*spot,am));
				pvol*=1.1;
				if (pvol>1) pvol=1;
				if (mode_==ask) {
					if (spot==vr.begin()) break;
					--spot;
				}
				else {
					++spot;
					if (spot==vr.end()) break;
				}

			}
			//front values first
			if (mode_==ask) {
				sort(begin(),end(),[&](const auto& i,const auto& j) -> bool { return i>j; });
			}
			else {
				sort(begin(),end(),[&](const auto& i,const auto& j) -> bool{ return i<j; });
			}

		}
		
	};
	void adjust_liquidity(const auto& vr, const spread& fx, int aperture_ticks) {
		auto mkt=_e.get_market_str(_cur1,_cur2);
		//wcout << "VALID RATES" << endl;
		//vr.dump(wcout);
		ostream nullos(0);
		auto ws=_e.get_wallets(_acid,_acpwd,nullos);

		auto dords=_e.get_orders(_acid,_acpwd);
		{
			omgr ords(dords,_cur1);
			myplan p(vr,ords,_cur1,ws.find(_cur1)->second,fx,plan::ask,aperture_ticks);
			p.execute(_e, _acid,_acpwd,_cur1,mkt,true); 
		}
		{
			omgr ords(dords,_cur2);
			myplan p(vr,ords,_cur2,ws.find(_cur2)->second,fx,plan::bid,aperture_ticks);
			p.execute(_e,_acid,_acpwd,_cur2,mkt,true); 
		}

	}

};

std::default_random_engine generator;


struct traders_manager: manager {
	traders_manager(engine& e, int cur1, double walletamount1, int cur2, double walletamount2, int num): manager(L"traders", e, cur1, num*walletamount1, cur2, num*walletamount2, 0), num(num), walletamount1(walletamount1), walletamount2(walletamount2) {
	}

	int num{0};
	int walletamount1{0};
	int walletamount2{0};

	void add(int n) {
		num+=n;
		ostream nullos(0);
		_e.deposit(_acid,_acpwd,_cur1,n*walletamount1,nullos);
		_e.deposit(_acid,_acpwd,_cur2,n*walletamount2,nullos);
	}

	struct myplan: plan {
		
		myplan(const auto& vr, omgr& orders, int cid, int num, double vol0, const auto& fx, mode mode_, int sigma_ticks, int aperture_ticks):plan(cid, orders, mode_) {
			auto spot=mode_==ask?vr.lower_rate(fx.ask):vr.upper_rate(fx.bid);

			if (spot==vr.end()) {
				return;
			}

			mode_==ask?spot-=aperture_ticks:spot+=aperture_ticks;
//wcout << " spot1=" << *spot << L" ask " << (mode_==ask) << endl;
			auto dist=distance(vr.begin(),spot);
			if (dist<0 || dist >=vr.size() ) {
				return;
			}


			double tick=curex::cex::decode_dbl(*(vr.begin()+1)-*vr.begin());

			std::normal_distribution<double> distribution(curex::cex::decode_dbl(*spot),sigma_ticks*tick);

			//wcout << L"-------------" << endl;
			int it=num>20*sigma_ticks?20*sigma_ticks:num;
			double vol=(double)num*vol0/(double)it;

			unordered_map<efp,double> x;
			for (int i=0; i<it; ++i) {
				auto number = distribution(generator);
				
				auto spot=mode_==ask?vr.lower_rate(number):vr.upper_rate(number);
				if (spot==vr.end()) continue;
				auto j=x.find(*spot);
				if (j==x.end()) {
					x.emplace(*spot,vol);
					continue;
				}
				j->second+=vol;
			}
			for (auto i:x) emplace_back(i.first,i.second);			
			//front values first
			if (mode_==ask) {
				sort(begin(),end(),[&](const auto& i,const auto& j) -> bool { return i>j; });
			}
			else {
				sort(begin(),end(),[&](const auto& i,const auto& j) -> bool{ return i<j; });
			}
/*
			auto b=spot-2*sigma_ticks;
			auto e=spot+2*sigma_ticks;
			for (auto i=b; i!=e; ++i) {
wcout << curex::cex::decode_dbl(*i) << L" d(i)=" << d(curex::cex::decode_dbl(*spot)) << L" totvol " << totvol << endl;
				emplace_back(pair<curex::cex::efp,double>(*i,totvol*d(curex::cex::decode_dbl(*i))));
			}  
*/
		}
		
	};
	void adjust_liquidity(const auto& vr, const spread& fx, double volcur1, double volcur2, int sigma_ticks, int aperture_ticks) {
		//wcout << "spread: " << fx.bid << " " << fx.ask << endl;
		ostream nullos(0);
		auto mkt=_e.get_market_str(_cur1,_cur2);

		int mpsz=0;
		auto dords=_e.get_orders(_acid,_acpwd);
		{
		omgr ords(dords,_cur1);
	//ords.dump(wcout);
		myplan p(vr,ords,_cur1,num, volcur1,fx,plan::ask,sigma_ticks,aperture_ticks);
//		p.dump(L" ",wcout);
	    p.execute(_e, _acid,_acpwd,_cur1,mkt,false); 
//		if (mpsz<p.size()) mpsz=p.size();
		}
		{
//		int cid=curex::cex::gbp::id;
		//wcout << L"plan GBP" << endl;
		omgr ords(dords,_cur2);
	//ords.dump(wcout);
		myplan p(vr,ords,_cur2,num, volcur2,fx,plan::bid,sigma_ticks,aperture_ticks);
//		p.dump(L" ",wcout);
//		plan& p=plancur2;
//		if (mpsz<p.size()) mpsz=p.size();
		p.execute(_e,_acid,_acpwd,_cur2,mkt,false); 
		}
		//_e.liquidity(eur::id,gbp::id,mpsz,wcout);
		//_e.status(_acid,_acpwd,wcout);
	}
};

using months=std::chrono::duration<int, std::ratio<3600*24*30>>;

int new_traders(double vol) {
	return vol/50000.0;
}


void burnout(int argc, char** argv) {
	if (argc!=2) {
		cout << "fx" << endl;
		exit(1);
	}
	std::string cmd=argv[1];
	if (cmd=="fx") {
		forex fx;
		fx.plot(cout);
		exit(0);
	}

	typedef curex::cex::ostream ostream;
	//engine::home=;
	//engine::init();
	ostream nullos(0);

	efp center=0;
	int width=0;

	engine e("",nullos);
	e.auto_match_is_not_free();
	e._system_account->delete_all_wallets();
	e.set_ruler(eur::id,gbp::id, 1.10,1.60,0.00005);
	curex::cex::string money0;
	forex fx;
	{
	investor_manager investor(e,eur::id,500000,gbp::id,500000);
	traders_manager traders(e,eur::id,1000,gbp::id,1000,0);
	{
	curex::cex::ostringstream os;
	os << "total money in the engine before activity:" << endl;
	e.total_money(os);
	money0=os.str();
	}

	int ii=0;

	std::ofstream minfile("min");
	std::ofstream dayfile("day");

	int tf=months(1)/t_units(1);

	auto vr=e.valid_rates(eur::id,gbp::id);
	int day=0;

	{
	const auto& sp=fx.get_spread(0);
	center=curex::cex::encode(sp.ask+(sp.bid-sp.ask)/2);
	width=200;

	}
	double minfx=1000;
	double maxfx=0;
	for (int t=0; t<tf; t+=40) {
		const auto& sp=fx.get_spread(t);
/*
auto sp=sp0;
	sp.bid=1.346;
	sp.ask=1.345;
if (day %2==0) {
	sp.bid+=0.001;
	sp.ask+=0.001;
}
	*/
		if (sp.ask<minfx) minfx=sp.ask;
		if (sp.bid>maxfx) maxfx=sp.bid;

		bool p=t%1440==0;
		if (p) {
	        wcout << L"-------- day " << day << L" " << (float)t/(float)tf << "% " << traders.num << endl;
			std::string dir;
			{
			std::ostringstream os;
			os << "days/day_" << day;
			dir=os.str();
			}
			{
			std::ostringstream os;
			os << "mkdir -p " << dir;
			system(os.str().c_str());
			}
			curex::cex::engine::stats st=e.get_stats();
			dayfile << t << " " << dir << " " << traders.num << " ";
			dayfile << sp.ask << " " << sp.bid << " ";
			st.dump_line(dayfile,eur::id);
			st.dump_line(dayfile,gbp::id);
			dayfile << endl;

			auto center2=curex::cex::encode(sp.ask+(sp.bid-sp.ask)/2);
			if (abs(center2-center)>200) center=center2;
// wcout << center << L" " << sp.ask << endl;
//wcout << center2 << L" " << sp.ask << endl;
			curex::cex::engine::info i=e.get_info(eur::id,gbp::id,center,width);
			{
			std::ostringstream os;
			os << dir << "/fx";
			std::ofstream f(os.str());
			i.lower_bound(curex::cex::encode(sp.ask))->second.second=true;
			i.upper_bound(curex::cex::encode(sp.bid))->second.second=true;
			f << std::distance(i.begin(),i.lower_bound(curex::cex::encode(sp.ask))) << " ";
			f << std::distance(i.begin(),i.upper_bound(curex::cex::encode(sp.bid))) << endl;
			}
			{
			std::ostringstream os;
			os << dir << "/market";
			std::ofstream f(os.str());
			i.write(f);
			}
			{
			std::ostringstream os;
			os << dir << "/ticks";
			std::ofstream f(os.str());
			auto range=i.range();
			auto g=i.gap();
			f << range.first << ", ";
			f << g.first << ", ";
			f << g.second << ", ";
			f << range.second << endl;
			}

			double volorders=0;
			curex::cex::engine::stats::t::const_iterator m;
			if ((m=st.money_in_orders.find(eur::id))!=st.money_in_orders.end()) volorders+=m->second;
			if ((m=st.money_in_orders.find(gbp::id))!=st.money_in_orders.end()) volorders+=m->second;

			int nt=new_traders(volorders);
			traders.add(nt); //grado de locura de un trader

			++day;
	        wcout << L"computing next day " << endl;
		}
		//e.set_ref_rate(eur::id,gbp::id,sp->bid+(sp->ask-sp->bid)/2); //adjust edges of rates

		minfile << t << " " << sp.ask << " " << traders.num << endl;

		int investor_ticks_aperture=3; ////shift of the higher rate bet towards safe side
		investor.adjust_liquidity(vr,sp,investor_ticks_aperture); //con 5 no hay matches de investor
		int traders_ticks_aperture=3;  //shift of the distribution mean towards safe side
		int traders_ticks_sigma=8; // width of the bet distribution
		traders.adjust_liquidity(vr,sp,100,100,traders_ticks_sigma,traders_ticks_aperture); //desv media

//		if (++ii==2) exit(0);
//		trs.do_trade(sp);

	}

	{
	std::ostringstream os;
	os << "fxrange";
	std::ofstream f(os.str());
	f << minfx-0.01 << ":" << maxfx+0.01 << endl;
	}


	}
	


	wcout << "System account" << endl;
	e._system_account->list(wcout);		

	curex::cex::string money1;
	{
	curex::cex::ostringstream os;
	os << "total money in the engine after activity:" << endl;
	e.total_money(os);
	money1=os.str();
	}

	wcout << money0 << endl;
	wcout << money1 << endl;

}


int main(int argc, char** argv) {


	burnout(argc,argv);
	return 0;
	double new_webusers_rate{10}; //10 a day
	double new_apiusers_rate{1}; //1 a day

	int active_webusers{0};
	int active_apiusers{0};
	
	double trade_commission=0.005;
	webuser wu;
	apiuser au;
	double profit_webusers{0};
	double profit_apiusers{0};

	for (int i=0; i<365; ++i) {
		active_webusers+=new_webusers_rate;
		active_apiusers+=new_apiusers_rate;
		profit_webusers+=wu.daily_profit(trade_commission);
		profit_apiusers+=au.daily_profit(trade_commission);
		double profit=profit_webusers+profit_apiusers;
		cout << "day " << i << " profit $" << profit << endl;
	}


return 0;
}


/*
*/
#if false
#include <mstd/algorithm>
struct trader {
	typedef curex::cex::string string;
	typedef curex::cex::id id;
	typedef curex::cex::efp efp;
	typedef curex::cex::ostream ostream;
	typedef curex::cex::engine::tradeinput tradeinput;
	typedef curex::cex::engine::updateinput updateinput;
	typedef curex::cex::ostringstream ostringstream;
	typedef curex::cex::istringstream istringstream;
	//cur1 weak, see acquire
	trader(engine& e, int cur1, int cur2, int gap_radious_ticks): _e(e), _cur1(cur1), _cur2(cur2), front_cur1(cur1,true), front_cur2(cur2,false), valid_rates(_e.valid_rates(cur1,cur2)) {
		mkt=_e.get_market_str(cur1,cur2);
		_acpwd=L"8888888888";
		ostream nullos(0);

		_acid=_e.new_account(_acpwd,nullos);
		_e.delete_all_wallets(_acid,_acpwd,nullos);
//		wcout << "trader acc id:" << _acid << endl;

		orders_cur1=new orders(_e,_acid,_acpwd,mkt,_cur1);
		orders_cur2=new orders(_e,_acid,_acpwd,mkt,_cur2);
//		trades_cur1=new trades(_e,_acid,_acpwd,mkt,_cur1);
//		trades_cur2=new trades(_e,_acid,_acpwd,mkt,_cur2);

		valid_rates.sort();
		tick=*(valid_rates.begin()+1)-*valid_rates.begin();
//wcout << shift << endl;



	}
	efp tick;
	trader(trader&& other): _e(other._e), _cur1(other._cur1), _cur2(other._cur2), mkt(move(other.mkt)), front_cur1(move(other.front_cur1)), front_cur2(move(other.front_cur2)), orders_cur1(other.orders_cur1), orders_cur2(other.orders_cur2), /*trades_cur1(other.trades_cur1), trades_cur2(other.trades_cur2),*/ _acpwd(move(other._acpwd)), _acid(other._acid), valid_rates(move(other.valid_rates)) {
		other.orders_cur1=0;
		other.orders_cur2=0;
//		other.trades_cur1=0;
//		other.trades_cur2=0;
	}
	trader(const trader& other)=delete;

	~trader() {
		if (orders_cur1!=0) {
			ostream nullos(0);
			_e.cancel_all_orders(_acid,_acpwd,nullos);
			wcout << "goodbye trader" << endl;
			_e.list_account(_acid,_acpwd,wcout);
		}
		//_e.status(_acid,_acpwd,wcout);
		delete orders_cur1;
		delete orders_cur2;
//		delete trades_cur1;
//		delete trades_cur2;
	}

	engine& _e;
	id _acid;
	string _acpwd;
	int _cur1, _cur2;
	string mkt;
	double ref_cur1,ref_cur2;

	curex::cex::market::spots valid_rates;

	double get_ref(int cid, const double& shift) const {
		if (cid==_cur1) return ref_cur1+shift;
		return ref_cur2+shift;
	}

	struct order {
		order(): _id(0), amount(0), rate(0) {}
		order(id id_,double am, efp rt): _id(id_), amount(am), rate(rt) {}
		id _id;
		double amount;
		efp rate;
		bool valid() const { return _id!=0; }
		void dump(ostream& os) const {
			os << _id << L" " << amount << L" " << rate << endl;
		} 
	};
	struct trade {
		trade(): amount(0), rate(0) {}
		trade(double am, efp rt): amount(am), rate(rt) {}
		double amount;
		efp rate;
		void dump(ostream& os) const {
			os << amount << L" " << rate << endl;
		} 
	};
	struct trades:std::vector<trade> {
		trades(engine& e, id acid, string pwd, string mkt, int cur): _e(e), acid(acid), pwd(pwd), _mkt(mkt), _cur(cur)  {

		}
		double get_sell_amount(const curex::cex::engine::dtrade& o) const {
			if (o.buy=='S') return o.get_amount();
			return o.get_amount2();
		}
		void dump(ostream& os) const {
			for (auto& i: *this) i.dump(os);
		} 
		void update(const curex::cex::engine::dtrades& mo) {
			const auto& o=mo.filter_front(_cur);
			clear();
			for (const auto& i:o) {
				emplace_back(trade(get_sell_amount(i),i.get_encoded_rate()));
			}
		}
		


		//M 121 B 80.00000 AUD 1.38995 AUD/USD 57.55600 USD 8.00000 AUD
		engine& _e;
		id acid;
		string pwd;
		int _cur;
		string _mkt;
	};

	struct orders: std::vector<order> {

		orders(engine& e, id acid, string pwd, string mkt, int cur): _e(e), acid(acid), pwd(pwd), _mkt(mkt), _cur(cur)  {

		}
		double get_sell_amount(const curex::cex::engine::dorder& o) const {
			if (o.buy=='S') return o.get_amount();
			//B 100.0000 GBP 2.0000 AUD/GBP
			//buy cur in the numerator
			if (o.market.find(o.cur)==0) return o.get_amount()/o.get_rate();
			//buy cur in the denominator
			return o.get_amount()*o.get_rate();

		}
		void dump(ostream& os) const {
			for (auto& i: *this) i.dump(os);
		} 

		void update(const curex::cex::engine::dorders& mo) {
			const auto& o=mo.filter_front(_cur);
			clear();
			for (const auto& i:o) {
				emplace_back(order(i._id,get_sell_amount(i),i.get_encoded_rate()));
			}
		}
		double get_exposure() const {
			return accumulate(begin(),end(),0.0,[](const auto& a, const auto& b) { return a+b.amount; });		
		}
		void reduce_exposure(double amount) {
			if (size()!=1) {
				dump(wcout);
				wcout << setprecision(10) << amount << endl;
				assert(false);
			}
			const auto& o=*begin();
			updateinput ui;

			ui.set_order_id(o._id);
			ui.set_amount(o.amount-amount);
			ui.set_rate(o.rate);
			ostream nullos(0);
			_e.update(acid, pwd, vector<updateinput>{ui}, nullos);		
		}
		void sell(efp rate, double amount) {
			std::vector<tradeinput> input;
			{
			tradeinput ti;
			ti.code=L"S";
			ti.set_amount(amount);
			ti.set_cur(_cur);
			ti.set_rate(rate);
			ti.rateunits=_mkt;
			input.push_back(ti);	
			}
			//U 1002 S 100.00000 EUR 1.28740 EUR/GBP

			curex::cex::ostringstream os;
			_e.trade(acid, pwd, input, os);
			curex::cex::string line;
			string v=os.str();
			curex::cex::istringstream is(v);
//wcout << os.str() << endl;
			//getline(is, line);					//U 1 S 500.00000 EUR 1.28755 EUR/GBP
			//wcout << "last line " << line << endl;
			curex::cex::string code;
			curex::cex::id id;
			curex::cex::string op;
			curex::cex::string am;
			is >> code;
			if (code==L"Error:") {
				if (v.find(L"E301;")!=string::npos || v.find(L"E11;")!=string::npos) {
					return;
				}
				wcout << v << endl;
				assert(false);
			}
			if(!code.empty()) {
				is >> id;
				is >> op;
				is >> am;
				assert(op==L"S");
				if(code==L"U") {
					push_back(order(id,curex::cex::decode_dbl(curex::cex::encode(am)),rate));
				}
				else if(code==L"M") {
					//update matches
				}
				
			}
		}
		void cancel_all() {
			clear();
			ostream nullos(0);

			_e.cancel_all_orders(acid, pwd, nullos);
		}
		void cancel_not_at(efp rate) {
			ostream nullos(0);

			mstd::filter(*this,[&](const auto&i) -> bool {
				if (i.rate==rate) return false;
				ostringstream os;
				_e.cancel_order(acid, pwd, i._id, os);
				string v=os.str();
				istringstream is(v);
				curex::cex::string c;
				is >> c;
				if (c==L"C") return true;
				if (c==L"Error:") {
					if (v.find(L"E18;")!=string::npos) {
						return true;
					}
					wcout << v << endl;
				}
				wcout << L"Unexpected result" << endl;
				abort();
				return false;
			});
		}
		engine& _e;
		id acid;
		string pwd;
		int _cur;
		string _mkt;
	};
	orders* orders_cur1;
	orders* orders_cur2;
	trades* trades_cur1;
	trades* trades_cur2;
	orders& get_orders(int cid) { 
		if (cid==_cur1) return *orders_cur1;
		return *orders_cur2;
	}
/*
	trades& get_trades(int cid) { 
		if (cid==_cur1) return *trades_cur1;
		return *trades_cur2;
	}
*/
	struct spot {
		spot(efp rate, double vol): rate(rate), vol(vol) {}
		efp rate;
		double vol;
	};
	struct front:vector<spot> {
		front(int cur, bool weak): cur(cur), weak( weak) {}
		front(front&&other): cur(other.cur), weak( other.weak), vector<spot>(other) {}
		bool weak;
		int cur;
		static spot nospot;
		const spot& dropback(double from) const {
			if (empty()) return nospot;
			return *begin();
		}
		void update(const curex::cex::engine::dliq& liq) {
			const auto& o=liq.filter_front(cur);
			clear();
			for (const auto& i:o) {
				emplace_back(spot(i.get_encoded_rate(),i.get_amount()));
			}
			//first entry is the closest to the gap
			sort(begin(),end(),[&](const auto&a,const auto&b) -> bool { return weak?a.rate>b.rate:a.rate<b.rate; });
		}
	};

	front front_cur1;
	front front_cur2;
	const front& get_front(int cid) const { 
		if (cid==_cur1) return front_cur1;
		return front_cur2;
	}

	efp get_sit_rate(int cid) const {
		 efp s=get_front(cid).dropback(get_ref(cid)).rate;
		 if (s!=0) return s;  //empty front
		 return get_front(cid).weak?*valid_rates.lower_rate(ref_cur1):*valid_rates.upper_rate(ref_cur2);
	}

	void risk(int cid, double target_exposure, double shift) {

		orders& o=get_orders(cid);
		if (target_exposure<0.1) {
			o.cancel_all();
			return;
		}
		efp sit=get_sit_rate(cid,shift);
		
		o.cancel_not_at(sit);
		double current_exposure=o.get_exposure();
		double diff=target_exposure-current_exposure;
		if (diff<-0.5) {
			o.reduce_exposure(-diff);
		}
		else if (diff>0.5) {
			o.sell(sit,diff);
		}
	}
//	int trend=0;
	
//	void update_trend() {
//		int a=rand()%100;
//		if (a>=50) ++trend;
//		else --trend;
//	}
	int gap_radious_ticks;
	void risk(int n) {
		double shift=((double)(tick*gap_radious_ticks))*((double)(rand()%100)/100.0)*(rand()%2==0?1:-1)/((double)curex::cex::one);		
//		update_trend();

		double exposure_cur1=100*n/2;
		double exposure_cur2=100*n/2;
		risk(_cur1,exposure_cur1,shift);
		risk(_cur2,exposure_cur2,shift);
	}

	void acquire(const spread& fx) {


		ref_cur1=fx.ask;
		ref_cur2=fx.bid;

		const curex::cex::engine::dorders dords=_e.get_orders(_acid,_acpwd);
		auto mdorders=dords.filter_market(mkt);
		orders_cur1->update(mdorders);
		orders_cur2->update(mdorders);
/*
		const curex::cex::engine::dtrades dtrds=_e.get_trades(_acid,_acpwd);
		auto mdtrades=dtrds.filter_market(mkt);
		trades_cur1->update(mdtrades);
		trades_cur2->update(mdtrades);
*/
		const curex::cex::engine::dliq dliq=_e.get_liquidity(_cur1,_cur2,8);
		front_cur1.update(dliq);
		front_cur2.update(dliq);

	}
	void do_trade(int n, const spread& fx) {
		acquire(fx);
		risk(n);
	}
};
trader::spot trader::front::nospot(0,0);


struct traders:trader {
	traders(engine& e, int cur1, int cur2, int gap_radious_ticks ): trader(_e,cur1,amount1,cur2,amount2, mkt, gap_radious_ticks) {

	}
	int num{0};
	void add_trader(double amount1, double amount2) {
		ostream nullos(0);
		_e.deposit(_acid,_acpwd,_cur1,amount1,nullos);
		_e.deposit(_acid,_acpwd,_cur2,amount2,nullos);
		++num;
	}


};


#endif



