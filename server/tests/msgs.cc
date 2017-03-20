#include <boost/test/unit_test.hpp>

#include "../src/msgs.hh"

#include <thread>
#include <vector>
#include <algorithm>

BOOST_AUTO_TEST_SUITE(msgs)

BOOST_AUTO_TEST_CASE( trivial )
{
	BOOST_CHECK(1);
}


// We'll write to this queue concurrently
// Then check if the results are nice

struct ThreadFunc
// The thread function that messes with the queue
{
	void operator()(std::string identity, std::string msg, Msg::Priority pri, unsigned num_msgs = 1000)
	{
		while (num_msgs > 0)
		{
			{
				auto q_lock = GlobalMsgQueue::get_lock();
				auto & q = GlobalMsgQueue::get_queue();
				BOOST_TEST_MESSAGE("Thd " << identity.c_str() << " CountDown " << num_msgs << " writing msg");
				q.emplace_back(msg, identity, pri);
			}
			num_msgs--;
		}
	}
};

BOOST_AUTO_TEST_CASE( concurrency )
{
	GlobalMsgQueue::init();

	const size_t num_threads = 32;
	const size_t msg_per_thd = 1000;

	std::vector<std::thread> threads(num_threads);
	const std::string default_msg ("abcdefghijklmnopqrstuvwxyz");
	const auto default_priority = Msg::Priority::Critical;

	BOOST_TEST_MESSAGE("Firing off " << std::to_string(num_threads).c_str() << " threads");

	for (size_t thd_id = 0; thd_id < num_threads; ++thd_id)
	{
		threads[thd_id] = std::thread((ThreadFunc()),
			std::to_string(thd_id), default_msg, default_priority, msg_per_thd);
	}

	BOOST_TEST_MESSAGE("Waiting for threads to finish");

	// Wait until completion
	for (std::thread & thd : threads)
	{
		thd.join();
	}

	BOOST_TEST_MESSAGE("Doing accounting...");

	// Count messages per thread and make sure constants (msg and pri) weren't corrupted
	MsgQueueWrapper::Queue & q = GlobalMsgQueue::get_queue();

	std::vector<size_t> num_msgs_per_thd(num_threads, 0);
	for (const Msg & msg : q)
	{
		size_t thd_id = atoi(msg.get_username().c_str());
		BOOST_CHECK(msg.get_msg() == default_msg);
		BOOST_CHECK(msg.get_pri() == default_priority);
		BOOST_CHECK(thd_id < num_threads);
		if (thd_id < num_threads)
		{
			++num_msgs_per_thd[thd_id];
		}
	}

	BOOST_CHECK( std::all_of(
		num_msgs_per_thd.cbegin(),
		num_msgs_per_thd.cend(),
		[msg_per_thd](size_t counter)
		{
			return counter == msg_per_thd;
		}) );

}

BOOST_AUTO_TEST_SUITE_END()




