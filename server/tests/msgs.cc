#include <boost/test/unit_test.hpp>

#include "../src/msgs.hh"

#include <thread>

BOOST_AUTO_TEST_SUITE(msgs)

BOOST_AUTO_TEST_CASE( trivial )
{
	BOOST_CHECK(1);
}

BOOST_AUTO_TEST_SUITE_END()


	// We'll write to this queue concurrently
	// Then check if the results are nice

