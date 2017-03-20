#include "reqs.hh"
#include "msgs.hh"

#include <regex>
#include <typeinfo>


namespace Cfg
{
	static bool regex_debug = false;
	static bool req_obj_debug = false;
	static bool req_obj_debug_verbose = false;
};


// Local Scope Helper Functions /////////////////////////////////////

namespace
{

enum class ReqType : unsigned
{
	NewLog,
	DumpAll,
	DeleteAll,
	Illegal
};

inline ReqType get_req_type(const std::string & req_type_str)
{
	ReqType type = ReqType::Illegal;

	static const std::vector<const char *> available_strings = {
		"new_log",
		"dump_all",
		"delete_all"
	};

	for (unsigned i = 0; i < available_strings.size(); ++i)
	{
		if (req_type_str == available_strings[i])
		{
			type = static_cast<ReqType>(i);
			break;
		}
	}
	return type;
}

std::string regex_debug(const std::smatch & matches)
{
	std::ostringstream oss;

	bool first = true;
	for (const auto & capture_group : matches)
	{
		if (!first)
			oss << "Captured: [" << capture_group << "]\n";
		first = false;
	}
	oss << "Suffix: [" << matches.suffix() << "]\n";

	return oss.str();
}

}


// Local Scope Class Definitions ////////////////////////////////////


class ReqNewLog : public ReqBase
{
public:

	static const char * req_type_str_static()
	{
		return "ReqNewLog";
	}

	virtual const char * req_type_str() const override
	{
		return ReqNewLog::req_type_str_static();
	}

	virtual void write_to_stream(std::ostream & os) const override
	{
		os << "[" << req_type_str() << "] [" << _msg << "]";
	}

	virtual ~ReqNewLog() override
	{
		if (Cfg::req_obj_debug)
		{
			std::cout << "ReqNewLog::dtor()\n";
		}
	}

	virtual void serve(MsgQueueWrapper & q_wrapper, std::ostream &) override
	{
		if (Cfg::req_obj_debug)
		{
			std::cout << "ReqNewLog::serve()\n";
		}

		auto lock = q_wrapper.get_lock();

		auto & q = q_wrapper.get_queue();
		q.push_back(std::move(_msg));
	}

	virtual bool operator==(const ReqBase & b_base) const override
	{
		// Return false if dynamic types are different
		if (typeid(*this) != typeid(b_base))
		{
			return false;
		}

		// Same dynamic type. Downcast then do member-wise comparison
		const ReqNewLog & b = static_cast<const ReqNewLog &>(b_base);

		return (this->_msg == b._msg);

	}

	static std::unique_ptr<ReqNewLog> make_req(const std::string & args, ReqUtils::ResultCode & result_code)
	{
		// Command Format
		// <username> <priority> <message_body>
		// username: Anything character that can be matched by (\S+), only printable & non-whitespace
		// priority: One of the following (low to high): debug, info, warning, critical, error
		// message body: anything that's printable! No trailing whitespace

		const unsigned MinGroups = 3;

		std::smatch matches;
		std::regex rule("^\\s*(\\S+)\\s+(\\S+)\\s+(.+)");

		std::regex_search(args, matches, rule);

		if (Cfg::regex_debug)
		{
			std::cout << "Request parse result level 2:\n";
			std::cout << regex_debug(matches);
		}

		if (matches.size() < MinGroups + 1)
		{
			result_code = ReqUtils::ResultCode::NewLogTooFewArgs;
			return nullptr;
		}

		const std::string & username = matches[1];
		const std::string & pri_str = matches[2];
		const std::string & msg_body = matches[3];

		const Msg::Priority pri = Msg::get_priority_from_str(pri_str);

		if (pri == Msg::Priority::Illegal)
		{
			result_code = ReqUtils::ResultCode::NewLogWrongPriority;
			return nullptr;
		}

		auto new_uptr = std::unique_ptr<ReqNewLog>(new ReqNewLog(msg_body, username, pri));

		result_code = ReqUtils::ResultCode::Ok;

		if (Cfg::req_obj_debug_verbose)
		{
			std::cout << *new_uptr << std::endl;
		}

		return new_uptr;
	}

	friend Reqs::ReqBaseUnqPtr ReqsUnitTestsUtils::make_new_log_req(
		const std::string & body,
		const std::string & username,
		const Msg::Priority priority);

private:
	ReqNewLog(
		const std::string & body,
		const std::string & username,
		const Msg::Priority priority)
	:
		_msg(body, username, priority)
	{
		if (Cfg::req_obj_debug)
		{
			std::cout << "ReqNewLog::ctor()\n";
		}
	}

	Msg _msg; // Non const cuz it's designed to be moved away after serve

};

class ReqDumpAll : public ReqBase
{
public:

	static const char * req_type_str_static()
	{
		return "ReqDumpAll";
	}

	virtual const char * req_type_str() const override
	{
		return ReqDumpAll::req_type_str_static();
	}

	virtual void write_to_stream(std::ostream & os) const override
	{
		os << "[" << req_type_str() << "] [" << Msg::get_priority_str(_pri) << "]";
	}

	virtual void serve(MsgQueueWrapper & q_wrapper, std::ostream & os) override
	{
		if (Cfg::req_obj_debug)
		{
			std::cout << "ReqDumpAll::serve()\n";
		}

		auto lock = q_wrapper.get_lock();

		q_wrapper.dump_to_stream(os, _pri);

	}

	virtual bool operator==(const ReqBase & b_base) const override
	{
		// Return false if dynamic types are different
		if (typeid(*this) != typeid(b_base))
		{
			return false;
		}

		// Same dynamic type. Downcast then do member-wise comparison
		const ReqDumpAll & b = static_cast<const ReqDumpAll &>(b_base);

		return (this->_pri == b._pri);

	}

	virtual ~ReqDumpAll() override
	{
		if (Cfg::req_obj_debug)
		{
			std::cout << "ReqDumpAll::dtor()\n";
		}
	}

	static std::unique_ptr<ReqDumpAll> make_req(const std::string & args, ReqUtils::ResultCode & result_code)
	{
		// Command Format:
		// dump_all <priority>
		//
		// priority: One of the following (low to high): debug, info, warning, critical, error

		const unsigned MinGroups = 1;

		std::smatch matches;
		std::regex rule("^\\s*(\\S+)\\s*");

		std::regex_search(args, matches, rule);

		if (Cfg::regex_debug)
		{
			std::cout << "Request parse result level 2:\n";
			std::cout << regex_debug(matches);
		}

		if (matches.size() < MinGroups + 1)
		{
			result_code = ReqUtils::ResultCode::DumpAllTooFewArgs;
			return nullptr;
		}

		const std::string & pri_str = matches[1];

		const Msg::Priority pri = Msg::get_priority_from_str(pri_str);

		if (pri == Msg::Priority::Illegal)
		{
			result_code = ReqUtils::ResultCode::DumpAllWrongPriority;
			return nullptr;
		}

		auto new_uptr = std::unique_ptr<ReqDumpAll>(new ReqDumpAll(pri));

		result_code = ReqUtils::ResultCode::Ok;

		if (Cfg::req_obj_debug_verbose)
		{
			std::cout << *new_uptr << std::endl;
		}

		return new_uptr;
	}

	friend Reqs::ReqBaseUnqPtr ReqsUnitTestsUtils::make_dump_all_req(const Msg::Priority pri);


private:
	ReqDumpAll(const Msg::Priority pri)
	:	_pri(pri)
	{
		if (Cfg::req_obj_debug)
		{
			std::cout << "ReqDumpAll::ctor()\n";
		}
	}

	const Msg::Priority _pri;

};

class ReqDeleteAll : public ReqBase
{
public:

	static const char * req_type_str_static()
	{
		return "ReqDeleteAll";
	}

	virtual const char * req_type_str() const override
	{
		return ReqDeleteAll::req_type_str_static();
	}

	virtual void write_to_stream(std::ostream & os) const override
	{
		os << "[" << req_type_str() << "]";
	}

	virtual void serve(MsgQueueWrapper & q_wrapper, std::ostream &) override
	{
		if (Cfg::req_obj_debug)
		{
			std::cout << "ReqDeleteAll::serve()\n";
		}

		auto lock = q_wrapper.get_lock();

		auto & q = q_wrapper.get_queue();
		q.clear();
	}

	virtual ~ReqDeleteAll() override
	{
		if (Cfg::req_obj_debug)
		{
			std::cout << "ReqDeleteAll::dtor()\n";
		}
	}

	virtual bool operator==(const ReqBase & b_base) const override
	{
		// Return false if dynamic types are different

		// Keep the syntax this way! For consistency with other subclasses.
		if (typeid(*this) != typeid(b_base))
		{
			return false;
		}

		return true;
	}

	static std::unique_ptr<ReqDeleteAll> make_req(ReqUtils::ResultCode & result_code)
	{
		// Command Format:
		// delete_all
		//

		auto new_uptr = std::unique_ptr<ReqDeleteAll>(new ReqDeleteAll);

		result_code = ReqUtils::ResultCode::Ok;

		if (Cfg::req_obj_debug_verbose)
		{
			std::cout << *new_uptr << std::endl;
		}

		return new_uptr;
	}

	friend Reqs::ReqBaseUnqPtr ReqsUnitTestsUtils::make_delete_all_req();


private:
	ReqDeleteAll()
	{
		if (Cfg::req_obj_debug)
		{
			std::cout << "ReqDeleteAll::ctor()\n";
		}
	}

};


// Exported Functions ///////////////////////////////////////////////


Reqs::ReqBaseUnqPtr Reqs::parse_req_str(const std::string & req_str, ReqUtils::ResultCode & result_code)
{
	// Step 1: Break down the request string.

	// String format:
	// <ReqType> <Args...>
	//		ReqType: One of the following: new_log, dump_all, delete_all
	//		Args: Send to subclass for processing. Must conform to the subtype's argument format

	// Set result string to an initial value...
	result_code = ReqUtils::ResultCode::UnknownError;

	const unsigned MinCaptures = 1;

	std::smatch matches;
	std::regex rule("^\\s*(\\S+)\\s*");

	std::regex_search(req_str, matches, rule);

	if (Cfg::regex_debug)
	{
		std::cout << "Request parse result level 1:\n";
		std::cout << regex_debug(matches);
	}

	// Step 2: Process first 2 common fields: request type, and username



	// Early exit due to not enough arguments?
	if (matches.size() < MinCaptures + 1)
		// Note: matches[0] is entire match. Capture groups start from i=1.
	{
		result_code = ReqUtils::ResultCode::TooFewArgs;
		return nullptr;
	}

	// First two fields correct. Keep going!
	const std::string & req_type_str = matches[1];
	//const std::string & username_str = matches[1];
	const std::string & additional_args = matches.suffix();

	ReqType req_type = get_req_type(req_type_str);

	ReqBaseUnqPtr req_obj_ptr;

	// Switch branches, delegate object construction to sub message classes
	// Ensure you set two fields correctly:
	//	- req_obj_ptr that points to a new request object if successful, or nullptr if not
	//	- result_str, some human-readable string telling users what's going on
	switch (req_type)
	{
		case ReqType::NewLog:
			req_obj_ptr = ReqBaseUnqPtr(ReqNewLog::make_req(additional_args, result_code));
			break;

		case ReqType::DumpAll:
			req_obj_ptr = ReqBaseUnqPtr(ReqDumpAll::make_req(additional_args, result_code));
			break;

		case ReqType::DeleteAll:
			req_obj_ptr = ReqBaseUnqPtr(ReqDeleteAll::make_req(result_code));
			break;

		case ReqType::Illegal:
		default:
			req_obj_ptr = nullptr;
			result_code = ReqUtils::ResultCode::UnknownReqType;
			break;
	}

	return req_obj_ptr;
}


namespace ReqsUnitTestsUtils
{

Reqs::ReqBaseUnqPtr make_new_log_req(
	const std::string & body,
	const std::string & username,
	const Msg::Priority priority)
{
	return Reqs::ReqBaseUnqPtr(new ReqNewLog(body, username, priority));
}

Reqs::ReqBaseUnqPtr make_dump_all_req(const Msg::Priority pri)
{
	return Reqs::ReqBaseUnqPtr(new ReqDumpAll(pri));
}

Reqs::ReqBaseUnqPtr make_delete_all_req()
{
	return Reqs::ReqBaseUnqPtr(new ReqDeleteAll);
}

};
