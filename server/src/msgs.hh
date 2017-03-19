#ifndef MSGS_HH
#define MSGS_HH

#include <vector>
#include <string>
#include <exception>
#include <iostream>


class Msg
{

public:
	typedef std::string MsgBody;
	typedef std::string Username;

	enum class Priority
	{
		Debug,
		Info,
		Warning,
		Critical,
		Error,
		Unknown
		// If you update this, don't forget to update the corresponding strings in msgs.cc
	};

	class InvalidPriorityException : public std::exception
	{
		virtual const char * what() const throw()
		{
			return "Invalid priority exception";
		}
	};

	static Priority get_priority_from_str(const std::string & priority_str)
	{
		Priority pri = Priority::Unknown;



		for (unsigned i = 0; i < _priority_strings.size(); ++i)
		{
			if (priority_str == _priority_strings[i])
			{
				pri = static_cast<Priority> (i);
				break;
			}
		}

		return pri;

	}

	static const char * get_priority_str(Priority pri)
	{
		return _priority_strings[static_cast<size_t>(pri)];
	};


	Msg(MsgBody body, Username username, Priority priority)
	:	_body(body),
		_username(username),
		_priority(priority)
	{
		if (_priority == Priority::Unknown)
		{
			throw InvalidPriorityException();
		}
	}

	friend std::ostream & operator<<(std::ostream & os, const Msg & msg)
	{
		os << "u[" << msg._username << "] p[" << Msg::get_priority_str(msg._priority) << "] m[" << msg._body << "]";
		return os;
	}


private:
	MsgBody _body;
	Username _username;
	Priority _priority;

	static const std::vector<const char *> _priority_strings;
};

#endif
