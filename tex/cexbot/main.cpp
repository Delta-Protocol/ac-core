#include <cexr/client.h>
#include <string>
#include <iostream>
#include <sstream>
#include <random>
#include <chrono>
#include <thread>

using namespace curex;
using namespace std;

int main() {
	string pwd="arbotante";
	client e;

	client::id aid=3; //e.new_account(pwd);
//	e.deposit(aid,pwd,1,"100000");
//	e.deposit(aid,pwd,2,"100000");
//	e.deposit(aid,pwd,3,"100000");
//	e.deposit(aid,pwd,4,"100000");

	std::minstd_rand gen(1729);
	auto mkts=e.get_markets();

while(true) {
	cout << "amos" << endl;


//	for (auto mkt:mkts) mkt.dump(cout);

//	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0,mkts.size()-1);
	int m = distribution(gen);
//cout << m << endl;
	mkts[m].dump(cout);

	string code=uniform_int_distribution<int>(0,1)(gen)==0?"B":"S";
	cout << "code " << code << endl;

	int amount=uniform_int_distribution<int>(1,1000000)(gen);
	double am=((double)amount)/10000;
	ostringstream a;
	a << am;

	cout << "amount " << a.str() << endl;

	string cur=uniform_int_distribution<int>(0,1)(gen)==0?mkts[m].weak_cur:mkts[m].strong_cur;
	cout << "cur " << cur << endl;


	auto vr=e.get_valid_rates(mkts[m].weak_curid,mkts[m].strong_curid);
	string rate=vr[uniform_int_distribution<int>(0,vr.size()-1)(gen)];
	cout << "rate " << rate << endl;

	vector<client::tradeinput> tiv;

	client::tradeinput ti;

	ti.id_of_client="A";
	ti.code=code;
	ti.amount=a.str();
	ti.cur=cur;
	ti.rate=rate;
	ti.rateunits=mkts[m].name;
	tiv.push_back(ti);
	string ans=e.trade(aid,pwd,tiv);
//	cout << ans;

//	std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(100));


	//cout << aid << endl;

}


}

