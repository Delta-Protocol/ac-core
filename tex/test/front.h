#ifndef CUREX_TEST_FRONT_
#define CUREX_TEST_FRONT_

#include <jail/unit_test.h>
#include <cex/front.h>


	class front: public ::test::unit_test {
		typedef ::test::unit_test B;
	public:
		front(counter&);
		void run();

	void test_encode();
	void test_populate_markets();
	void match_same_account_single_spot();
	void match_same_account_two_spots();
	void order_buy_and_sell_on_two_fronts_in_a_single_trade();
	void trade_two_accounts();
	void trade_by_sellA_sellB_single_spot(bool disk);
	void trade_by_sellB_sellA_single_spot();
	void trade_by_buyA_buyB_single_spot();
	void trade_by_buyB_buyA_single_spot();
	void trade_by_sellA_buyA_single_spot();
	void trade_by_buyA_sellA_single_spot();
	void trade_by_sellB_buyB_single_spot();
	void trade_by_buyB_sellB_single_spot();

	void test_restart_engine(std::string home,curex::cex::id id1,curex::cex::id id2);

	void test_concurrency();
	void tradelow();
	

	};


#endif

