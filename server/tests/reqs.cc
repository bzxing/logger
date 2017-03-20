
#include <boost/test/unit_test.hpp>


#include "../src/reqs.hh"
#include "../src/msgs.hh"

#include "utils.hh"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

#include <thread>

#include <cstdlib>
#include <ctime>
#include <cassert>

BOOST_AUTO_TEST_SUITE( reqs )

const unsigned rand_seed = 0;

BOOST_AUTO_TEST_CASE( good_new_logs )
{
	srand(rand_seed);
	const unsigned num_cases = 500;

	BOOST_TEST_MESSAGE("Checking correct generation of new_log requests from " << std::to_string(num_cases).c_str() << " random strings");

	for (unsigned i = 0; i < num_cases; ++i)
	{
		const std::string req_name = "new_log";
		const std::string username = random_token();
		const auto p = random_pri();
		const std::string priority_str = Msg::get_priority_str(p);
		const std::string message = random_msg();

		std::string req_str;
		req_str = random_whitespace() + req_name +
			random_whitespace() + username +
			random_whitespace() + priority_str +
			random_whitespace() + message;

		//BOOST_TEST_MESSAGE ("Req: [" << req_str.c_str() << "]");
		ReqUtils::ResultCode ec;
		auto req_ptr = Reqs::parse_req_str(req_str, ec);
		//BOOST_TEST_MESSAGE("\tEC: " << ReqUtils::get_short_result_str(ec) << "");
		BOOST_CHECK(req_ptr != nullptr);
		if (req_ptr == nullptr)
		{
			continue;
		}
		BOOST_CHECK( *req_ptr ==
			*ReqsUnitTestsUtils::make_new_log_req(message, username, p) );
		BOOST_CHECK( ec == ReqUtils::ResultCode::Ok );
	}
}

BOOST_AUTO_TEST_CASE( good_dump_all )
{
	srand(rand_seed);

	const unsigned num_cases = 500;

	BOOST_TEST_MESSAGE("Checking correct generation of dump_all requests from " << std::to_string(num_cases).c_str() << " random strings");

	for (unsigned i = 0; i < num_cases; ++i)
	{
		const std::string req_name = "dump_all";
		const auto p = random_pri();
		const std::string priority_str = Msg::get_priority_str(p);

		std::string req_str;
		req_str = random_whitespace() + req_name +
			random_whitespace() + priority_str;

		//BOOST_TEST_MESSAGE ("Req: [" << req_str.c_str() << "]");
		ReqUtils::ResultCode ec;
		auto req_ptr = Reqs::parse_req_str(req_str, ec);
		//BOOST_TEST_MESSAGE("\tEC: " << ReqUtils::get_short_result_str(ec) << "");
		BOOST_CHECK(req_ptr != nullptr);
		if (req_ptr == nullptr)
		{
			continue;
		}
		BOOST_CHECK( *req_ptr ==
			*ReqsUnitTestsUtils::make_dump_all_req(p) );
		BOOST_CHECK( ec == ReqUtils::ResultCode::Ok );
	}
}

BOOST_AUTO_TEST_CASE( good_delete_all )
{
	BOOST_TEST_MESSAGE("Checking correct generation of delete_all requests from strings");
	std::string req_str("  delete_all");

	//BOOST_TEST_MESSAGE ("Req: [" << req_str.c_str() << "]");
	ReqUtils::ResultCode ec;
	auto req_ptr = Reqs::parse_req_str(req_str, ec);
	//BOOST_TEST_MESSAGE("\tEC: " << ReqUtils::get_short_result_str(ec) << "");
	BOOST_CHECK(req_ptr != nullptr);

	BOOST_CHECK( *req_ptr ==
		*ReqsUnitTestsUtils::make_delete_all_req() );
	BOOST_CHECK( ec == ReqUtils::ResultCode::Ok );
}


BOOST_AUTO_TEST_CASE( bad_reqs )
{
	static const std::vector<std::pair<std::string, ReqUtils::ResultCode>> bad_reqs =
		{
			std::make_pair("new_log", ReqUtils::ResultCode::NewLogTooFewArgs),
			std::make_pair("new_log Joe critical", ReqUtils::ResultCode::NewLogTooFewArgs),
			std::make_pair("new_log Joe wrong_priority important_message", ReqUtils::ResultCode::NewLogWrongPriority),
			std::make_pair("dump_all", ReqUtils::ResultCode::DumpAllTooFewArgs),
			std::make_pair("dump_all haha_this_priority_doesnt_exist", ReqUtils::ResultCode::DumpAllWrongPriority),
			std::make_pair("magic", ReqUtils::ResultCode::UnknownReqType),
			std::make_pair("areyoukiddingmeicanteventhinkofagoodtestcase", ReqUtils::ResultCode::UnknownReqType)
		};

	BOOST_TEST_MESSAGE("Checking whether " << std::to_string(bad_reqs.size()) << " bad request strings are caught properly");
	for (const auto & pair : bad_reqs)
	{
		ReqUtils::ResultCode ec;
		//BOOST_TEST_MESSAGE("Req: [" << pair.first.c_str() << "]");
		auto req_ptr = Reqs::parse_req_str(pair.first, ec);
		//BOOST_TEST_MESSAGE("\tEC: " << ReqUtils::get_short_result_str(ec) << "");
		BOOST_CHECK(req_ptr == nullptr);
		BOOST_CHECK(ec == pair.second);
	}

}

BOOST_AUTO_TEST_CASE( serve_reqs )
{
	GlobalMsgQueue::init();
	srand(rand_seed);

	auto & q_wrapper = GlobalMsgQueue::get_inst();
	auto & q = GlobalMsgQueue::get_queue();
	q.clear();

	// New Log: Read and Write test
	const unsigned num_thds = 32;
	const unsigned num_msg_at_each_priority_per_thd = 2000;
	const unsigned num_read_and_write = unsigned(Msg::Priority::Max) * num_msg_at_each_priority_per_thd;
	BOOST_TEST_MESSAGE("Testing serving new log requests concurrently");


	struct ThdFunc
	{
		void operator()(size_t num_read_and_write)
		{
			for (unsigned i = 0; i < num_read_and_write; ++i)
			{

				const std::string username = random_token();
				const auto p = Msg::Priority(i % unsigned(Msg::Priority::Max));
				const std::string message = random_msg();

				auto req_ptr = ReqsUnitTestsUtils::make_new_log_req(message, username, p);

				auto & q_wrapper = GlobalMsgQueue::get_inst();
				auto & q = GlobalMsgQueue::get_queue();
				std::ostringstream oss;

				auto lock = req_ptr->serve(q_wrapper, oss);

				auto msg = Msg(message, username, p);

				BOOST_CHECK(msg == q.back());
			}
		}
	};

	BOOST_TEST_MESSAGE("Lauching " << std::to_string(num_thds).c_str() << "threads and let them finish...");

	std::vector<std::thread> threads(num_thds);
	for (std::thread & thd : threads)
	{
		thd = std::thread((ThdFunc()), num_read_and_write);
	}


	for (std::thread & thd : threads)
	{
		thd.join();
	}


	BOOST_TEST_MESSAGE("Checking msg count at each priority using dump_all");
	// Test dump_all at each priority leve
	for (unsigned i = 0; i < unsigned(Msg::Priority::Max); i++)
	{
		auto p = Msg::Priority(i);
		auto dump_all_req_ptr = ReqsUnitTestsUtils::make_dump_all_req(p);

		std::stringstream ss;
		dump_all_req_ptr->serve(q_wrapper, ss);

		std::string line;
		size_t num_lines = 0;
		while (std::getline(ss, line))
		{
			++num_lines;
		}

		BOOST_CHECK(num_lines == (unsigned(Msg::Priority::Max) - i) * num_msg_at_each_priority_per_thd * num_thds);
	}

	BOOST_TEST_MESSAGE("Testint delete_all request");
	// Erase everything
	std::ostringstream oss;
	auto del_all_req_ptr = ReqsUnitTestsUtils::make_delete_all_req();
	del_all_req_ptr->serve(q_wrapper, oss);
	BOOST_CHECK(q.empty());

}

BOOST_AUTO_TEST_SUITE_END()
