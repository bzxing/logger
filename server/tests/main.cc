#define BOOST_TEST_MODULE MyTest
#include <boost/test/included/unit_test.hpp>


#include "../src/reqs.hh"

#include <iostream>
#include <sstream>
#include <string>



BOOST_AUTO_TEST_CASE( my_test )
{
	ReqUtils::ResultCode ec;
	auto req_ptr = Reqs::parse_req_str(
		" \tnew_log   \t billxing  \t  critical  \t Hello world!",
		ec);
	BOOST_CHECK(req_ptr != nullptr);

	std::ostringstream oss;
	oss << *req_ptr;
	std::string expected("[ReqNewLog] [u[billxing] p[critical] m[Hello world!]]");
	std::cout << "Expected: " << expected << std::endl;
	std::cout << "Returned: " << *req_ptr << std::endl;
	BOOST_CHECK( oss.str() == expected );
}
