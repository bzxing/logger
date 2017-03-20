
#include <boost/test/unit_test.hpp>


#include "../src/reqs.hh"
#include "../src/msgs.hh"

#include "utils.hh"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>
#include <ctime>
#include <cassert>

BOOST_AUTO_TEST_SUITE( reqs )

BOOST_AUTO_TEST_CASE( good_new_logs )
{
	srand(time(NULL));
	const unsigned num_cases = 50;
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

		BOOST_TEST_MESSAGE ("Req: [" << req_str.c_str() << "]");
		ReqUtils::ResultCode ec;
		auto req_ptr = Reqs::parse_req_str(req_str, ec);
		BOOST_TEST_MESSAGE("\tEC: " << ReqUtils::get_short_result_str(ec) << "");
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
	srand(time(NULL));
	const unsigned num_cases = 50;
	for (unsigned i = 0; i < num_cases; ++i)
	{
		const std::string req_name = "dump_all";
		const auto p = random_pri();
		const std::string priority_str = Msg::get_priority_str(p);

		std::string req_str;
		req_str = random_whitespace() + req_name +
			random_whitespace() + priority_str;

		BOOST_TEST_MESSAGE ("Req: [" << req_str.c_str() << "]");
		ReqUtils::ResultCode ec;
		auto req_ptr = Reqs::parse_req_str(req_str, ec);
		BOOST_TEST_MESSAGE("\tEC: " << ReqUtils::get_short_result_str(ec) << "");
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
	std::string req_str("  delete_all");

	BOOST_TEST_MESSAGE ("Req: [" << req_str.c_str() << "]");
	ReqUtils::ResultCode ec;
	auto req_ptr = Reqs::parse_req_str(req_str, ec);
	BOOST_TEST_MESSAGE("\tEC: " << ReqUtils::get_short_result_str(ec) << "");
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

	for (const auto & pair : bad_reqs)
	{
		ReqUtils::ResultCode ec;
		BOOST_TEST_MESSAGE("Req: [" << pair.first.c_str() << "]");
		auto req_ptr = Reqs::parse_req_str(pair.first, ec);
		BOOST_TEST_MESSAGE("\tEC: " << ReqUtils::get_short_result_str(ec) << "");
		BOOST_CHECK(req_ptr == nullptr);
		BOOST_CHECK(ec == pair.second);
	}

}

BOOST_AUTO_TEST_SUITE_END()
