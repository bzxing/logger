
#ifndef REQS_HH
#define REQS_HH

#include <string>
#include <iostream>
#include <regex>
#include <memory>

#include "msgs.hh"


namespace Cfg
{
	static bool regex_debug = true;
	static bool req_obj_debug = true;
	static bool req_obj_debug_verbose = false;
};

class ReqCommon
{
public:
	typedef std::string Username;

	enum class ReqType : unsigned
	{
		NewLog,
		DumpAll,
		DeleteAll,
		Unknown
	};


	enum class ResultCode : unsigned
	{
		Ok,
		TooFewArgs,
		NewLogTooFewArgs,
		NewLogWrongPriority,
		DumpAllTooFewArgs,
		DumpAllWrongPriority,
		UnknownReqType,
		UnknownError
	};

	static const char * get_short_result_str(ResultCode result_code)
	{
		static const std::vector<const char *> available_strings = {
			"Ok",
			"TooFewArgs",
			"NewLogTooFewArgs",
			"NewLogWrongPriority",
			"DumpAllTooFewArgs",
			"DumpAllWrongPriority",
			"UnknownReqType",
			"UnknownError"
		};

		return available_strings[ static_cast<size_t>(result_code) ];
	}

	static const char * get_result_str(ResultCode result_code)
	{
		static const std::vector<const char *> available_strings = {
			"Big success!",

			"Too few arguments. Please start the request with argument: <request_type>, "
				"which can be one of the following: new_log, dump_all, delete_all",

			"Too few arguments for request type \"new_log\". Usage: "
				"new_log <username (no whitespace)> <priority (debug/info/warning/critical/error)> "
					"<message body, anything but no leading or trailing whitespace>",

			"Invalid priority string for request type \"new_log\". Choose one of the following: "
				"debug/info/warning/critical/error",

			"Too few arguments for request type \"dump_all\". Usage: "
				"dump_all <priority (debug/info/warning/critical/error)>",

			"Invalid priority string for request type \"dump_all\". Choose one of the following: "
				"debug/info/warning/critical/error",


			"Unknown request type. Available request types are: new_log, dump_all, delete_all",

			"Unknown error. Sorry, poor user."
		};

		return available_strings[ static_cast<size_t>(result_code) ];
	}

	static std::string regex_debug(const std::smatch & matches)
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

};


class ReqBase
{

public:

	virtual const char * req_type_str() const = 0;

	virtual void write_to_stream(std::ostream &) const = 0;

	virtual ~ReqBase() = default;

	virtual void serve() = 0;

	friend std::ostream & operator<<(std::ostream & os, const ReqBase & req)
	{
		req.write_to_stream(os);
		return os;
	}

private:

};


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

	virtual void serve() override
	{
		if (Cfg::req_obj_debug)
		{
			std::cout << "ReqNewLog::serve()\n";
		}
	}

	static std::unique_ptr<ReqNewLog> make_req(const std::string & args, ReqCommon::ResultCode & result_code)
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
			std::cout << ReqCommon::regex_debug(matches);
		}

		if (matches.size() < MinGroups + 1)
		{
			result_code = ReqCommon::ResultCode::NewLogTooFewArgs;
			return nullptr;
		}

		const std::string & username = matches[1];
		const std::string & pri_str = matches[2];
		const std::string & msg_body = matches[3];

		const Msg::Priority pri = Msg::get_priority_from_str(pri_str);

		if (pri == Msg::Priority::Unknown)
		{
			result_code = ReqCommon::ResultCode::NewLogWrongPriority;
			return nullptr;
		}

		auto new_uptr = std::unique_ptr<ReqNewLog>(new ReqNewLog(msg_body, username, pri));

		result_code = ReqCommon::ResultCode::Ok;

		if (Cfg::req_obj_debug_verbose)
		{
			std::cout << *new_uptr << std::endl;
		}

		return new_uptr;
	}



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

	const Msg _msg;

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

	virtual void serve() override
	{
		if (Cfg::req_obj_debug)
		{
			std::cout << "ReqDumpAll::serve()\n";
		}
	}

	virtual ~ReqDumpAll() override
	{
		if (Cfg::req_obj_debug)
		{
			std::cout << "ReqDumpAll::dtor()\n";
		}
	}

	static std::unique_ptr<ReqDumpAll> make_req(const std::string & args, ReqCommon::ResultCode & result_code)
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
			std::cout << ReqCommon::regex_debug(matches);
		}

		if (matches.size() < MinGroups + 1)
		{
			result_code = ReqCommon::ResultCode::DumpAllTooFewArgs;
			return nullptr;
		}

		const std::string & pri_str = matches[1];

		const Msg::Priority pri = Msg::get_priority_from_str(pri_str);

		if (pri == Msg::Priority::Unknown)
		{
			result_code = ReqCommon::ResultCode::DumpAllWrongPriority;
			return nullptr;
		}

		auto new_uptr = std::unique_ptr<ReqDumpAll>(new ReqDumpAll(pri));

		result_code = ReqCommon::ResultCode::Ok;

		if (Cfg::req_obj_debug_verbose)
		{
			std::cout << *new_uptr << std::endl;
		}

		return new_uptr;
	}


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

	virtual void serve() override
	{
		if (Cfg::req_obj_debug)
		{
			std::cout << "ReqDeleteAll::serve()\n";
		}
	}

	virtual ~ReqDeleteAll() override
	{
		if (Cfg::req_obj_debug)
		{
			std::cout << "ReqDeleteAll::dtor()\n";
		}
	}

	static std::unique_ptr<ReqDeleteAll> make_req(ReqCommon::ResultCode & result_code)
	{
		// Command Format:
		// delete_all
		//

		auto new_uptr = std::unique_ptr<ReqDeleteAll>(new ReqDeleteAll());

		result_code = ReqCommon::ResultCode::Ok;

		if (Cfg::req_obj_debug_verbose)
		{
			std::cout << *new_uptr << std::endl;
		}

		return new_uptr;
	}




private:
	ReqDeleteAll()
	{
		if (Cfg::req_obj_debug)
		{
			std::cout << "ReqDeleteAll::ctor()\n";
		}
	}

};


class Reqs
{

public:

	typedef ReqBase * ReqBasePtr;
	typedef std::unique_ptr<ReqBase> ReqBaseUnqPtr;
	// typedef ThreadedQueue< ReqBaseUnqPtr > ReqQueue;

	static ReqBaseUnqPtr parse_req_str(const std::string & req_str, ReqCommon::ResultCode & result_code)
	{
		// Step 1: Break down the request string.

		// String format:
		// <ReqType> <Args...>
		//		ReqType: One of the following: new_log, dump_all, delete_all
		//		Args: Send to subclass for processing. Must conform to the subtype's argument format

		// Set result string to an initial value...
		result_code = ReqCommon::ResultCode::UnknownError;

		const unsigned MinCaptures = 1;

		std::smatch matches;
		std::regex rule("^\\s*(\\S+)\\s*");

		std::regex_search(req_str, matches, rule);

		if (Cfg::regex_debug)
		{
			std::cout << "Request parse result level 1:\n";
			std::cout << ReqCommon::regex_debug(matches);
		}

		// Step 2: Process first 2 common fields: request type, and username



		// Early exit due to not enough arguments?
		if (matches.size() < MinCaptures + 1)
			// Note: matches[0] is entire match. Capture groups start from i=1.
		{
			result_code = ReqCommon::ResultCode::TooFewArgs;
			return nullptr;
		}

		// First two fields correct. Keep going!
		const std::string & req_type_str = matches[1];
		//const std::string & username_str = matches[1];
		const std::string & additional_args = matches.suffix();

		ReqCommon::ReqType req_type = get_req_type(req_type_str);

		ReqBaseUnqPtr req_obj_ptr;

		// Switch branches, delegate object construction to sub message classes
		// Ensure you set two fields correctly:
		//	- req_obj_ptr that points to a new request object if successful, or nullptr if not
		//	- result_str, some human-readable string telling users what's going on
		switch (req_type)
		{
			case ReqCommon::ReqType::NewLog:
				req_obj_ptr = ReqBaseUnqPtr(ReqNewLog::make_req(additional_args, result_code));
				break;

			case ReqCommon::ReqType::DumpAll:
				req_obj_ptr = ReqBaseUnqPtr(ReqDumpAll::make_req(additional_args, result_code));
				break;

			case ReqCommon::ReqType::DeleteAll:
				req_obj_ptr = ReqBaseUnqPtr(ReqDeleteAll::make_req(result_code));
				break;

			case ReqCommon::ReqType::Unknown:
			default:
				req_obj_ptr = nullptr;
				result_code = ReqCommon::ResultCode::UnknownReqType;
				break;
		}

		return req_obj_ptr;
	}


private:

	static ReqCommon::ReqType get_req_type(const std::string & req_type_str)
	{
		ReqCommon::ReqType type = ReqCommon::ReqType::Unknown;

		static const std::vector<const char *> available_strings = {
			"new_log",
			"dump_all",
			"delete_all"
		};

		for (unsigned i = 0; i < available_strings.size(); ++i)
		{
			if (req_type_str == available_strings[i])
			{
				type = static_cast<ReqCommon::ReqType>(i);
				break;
			}
		}
		return type;
	}

	//ReqQueue _req_queue;

	//static Reqs _reqs; // Links to the global request queue
};

#endif


