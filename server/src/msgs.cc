#include "msgs.hh"

#include <vector>

const std::vector<const char *> Msg::_priority_strings = {
		"debug",
		"info",
		"warning",
		"critical",
		"error",
		"illegal"
	};

// Global msg queue.
std::unique_ptr<MsgQueueWrapper> GlobalMsgQueue::_inst;

