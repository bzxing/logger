#define BOOST_TEST_MODULE ServerRequests
#include <boost/test/included/unit_test.hpp>


#include "../src/reqs.hh"
#include "../src/msgs.hh"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>
#include <ctime>


BOOST_AUTO_TEST_SUITE( new_log_reqs )

std::string random_whitespace(unsigned max_len = 10)
{
	unsigned len = (unsigned)rand() % (max_len-1) + 1;
	std::string white;
	while (len != 0)
	{
		white += " ";
		--len;
	}
	return white;
}


BOOST_AUTO_TEST_CASE( good_new_logs )
{
	srand(time(NULL));
	for (unsigned p = 0; p < unsigned(Msg::Priority::Max); ++p)
	{


		const char * req_name = "new_log";
		const char * username = "MyNameIsBillXing";
		const char * priority_str = Msg::get_priority_str(Msg::Priority(p));
		const char * message = "Hello world! Here's my message. Deal with it!!";

		std::string req_str;
		req_str = random_whitespace() + req_name +
			random_whitespace() + username +
			random_whitespace() + priority_str +
			random_whitespace() + message;

		std::cout << "Req: [" << req_str << "]\n";
		ReqUtils::ResultCode ec;
		auto req_ptr = Reqs::parse_req_str(req_str, ec);
		std::cout << "\tEC: " << ReqUtils::get_short_result_str(ec) << std::endl;
		BOOST_CHECK(req_ptr != nullptr);
		BOOST_CHECK( *req_ptr ==
			*ReqsUnitTestsUtils::make_new_log_req(message, username, Msg::Priority(p)) );
		BOOST_CHECK( ec == ReqUtils::ResultCode::Ok );
	}
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
		std::cout << "Req: [" << pair.first << "]\n";
		auto req_ptr = Reqs::parse_req_str(pair.first, ec);
		std::cout << "\tEC: " << ReqUtils::get_short_result_str(ec) << "\n";
		BOOST_CHECK(req_ptr == nullptr);
		BOOST_CHECK(ec == pair.second);
	}

}

BOOST_AUTO_TEST_SUITE_END()
