#define BOOST_TEST_MODULE ServerTests

#include <boost/test/included/unit_test.hpp>

#include "../src/msgs.hh"

bool init_unit_test()
{
	srand(time(NULL));
	GlobalMsgQueue::init();
	return true;
}

