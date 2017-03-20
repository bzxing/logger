
#ifndef REQS_HH
#define REQS_HH

#include <string>
#include <iostream>
#include <memory>
#include <vector>

#include "msgs.hh"


class MsgQueueWrapper;
class GlobalMsgQueue;

namespace ReqUtils
{

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

	inline const char * get_short_result_str(ResultCode result_code)
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

	inline const char * get_result_str(ResultCode result_code)
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
};


class ReqBase
{

public:

	virtual const char * req_type_str() const = 0;

	virtual void write_to_stream(std::ostream &) const = 0;

	virtual ~ReqBase() = default;

	virtual MsgQueueWrapper::Lock serve(MsgQueueWrapper &, std::ostream &) = 0;

	friend std::ostream & operator<<(std::ostream & os, const ReqBase & req)
	{
		req.write_to_stream(os);
		return os;
	}

	virtual bool operator==(const ReqBase &) const = 0;

	bool operator!=(const ReqBase & b) const
	{
		return !(*this == b);
	}

private:

};


namespace Reqs
{
	typedef std::unique_ptr<ReqBase> ReqBaseUnqPtr;

	ReqBaseUnqPtr parse_req_str(const std::string & req_str, ReqUtils::ResultCode & result_code);

};


namespace ReqsUnitTestsUtils
{
	Reqs::ReqBaseUnqPtr make_new_log_req(
		const std::string & body,
		const std::string & username,
		const Msg::Priority priority);

	Reqs::ReqBaseUnqPtr make_dump_all_req(const Msg::Priority pri);

	Reqs::ReqBaseUnqPtr make_delete_all_req();
};





#endif


