
#ifndef TEST_UTILS_HH
#define TEST_UTILS_HH

#include "../src/msgs.hh"

#include <string>

std::string random_whitespace(unsigned max_len = 5);

std::string random_token(unsigned max_len = 8);

Msg::Priority random_pri();

std::string random_msg(unsigned max_tokens = 10);

#endif
