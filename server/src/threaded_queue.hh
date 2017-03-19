
#ifndef THREADED_QUEUE_HH
#define THREADED_QUEUE_HH

#include <string>
#include <deque>

#include <thread>

#include <exception>


// Types ////////////////////////////////////////////////////////////
//typedef ThreadedQueue<std::string> ReqQueue;


// Body /////////////////////////////////////////////////////////////

template <class Msg>
class ThreadedQueue
// Provide atomic operations on std::queue that holds any type :)
{

public:
	typedef std::mutex Mutex;

	class EmptyQueueException : public std::exception
	{
		virtual const char * what() const throw()
		{
			return "Empty queue exception."
		}
	}

	void push_back(const Msg & msg)
	{
		std::unique_lock(_mutex);
		_queue.push_back(msg);
	}

	void push_back(Msg && msg)
	{
		std::unique_lock(_mutex);
		_queue.push_back(std::move(msg));
	}
/*
	void pop_front()
	{
		std::unique_lock(_mutex);
		_queue.pop_front();
	}

	bool empty() const
	{
		std::unique_lock(_mutex);
		return _queue.empty();
	}
*/
	Msg pop_front()
	{
		std::unique_lock(_mutex);

		if (_queue.empty())
		{
			throw EmptyQueueException;
		}

		Msg msg = std::move(_queue.front());
		_queue.pop_front();

		return msg;
	}

	void clear()
	{
		std::unique_lock(_mutex);

		_queue.clear();
	}
/*
	Msg back() const
	{
		std::unique_lock(_mutex);
		if (_queue.empty())
		{
			throw std::out_of_range("Read from empty queue!")
		}
		return _queue.back();
	}

	Msg operator[](size_t idx) const
	// Don't provide a non-const version 'cuz that's thread unsafe
	{
		std::unique_lock(_mutex);
		if (_queue.empty())
		{
			throw std::out_of_range("Read from empty queue!")
		}
		return _queue[idx];
	}
*/
private:

	std::deque<Msg> _queue;
	Mutex _mutex;
};



// Template Instantiations //////////////////////////////////////////

#endif
