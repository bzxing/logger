
#include "utils.hh"
#include "../src/msgs.hh"

#include <cstdlib>
#include <cassert>

#include <string>

std::string random_whitespace(unsigned max_len)
{
	unsigned len = (unsigned)rand() % (max_len-1) + 1;
	std::string white;
	while (len != 0)
	{
		white += " ";
		--len;
	}
	return white;
}

std::string random_token(unsigned max_len)
{
	unsigned len = (unsigned)rand() % (max_len-1) + 1;

	const unsigned char char_min = '!';
	const unsigned char char_max = '~';
	assert(char_max > char_min); // Should never reach here

	const unsigned char char_range = char_max - char_min;


	std::string random_token;
	while (len != 0)
	{
		unsigned char offset = (unsigned char)rand() % char_range;
		unsigned char random_char = char_min + offset;
		random_token += char(random_char);
		--len;
	}
	return random_token;
}

Msg::Priority random_pri()
{
	unsigned max = unsigned (Msg::Priority::Max);
	unsigned p = (unsigned)rand() % max;
	return Msg::Priority(p);
}

std::string random_msg(unsigned max_tokens )
{
	unsigned ntokens = (unsigned)rand() % (max_tokens-1) + 1;

	std::string my_msg;

	for (unsigned i = 0; i < ntokens; ++i)
	{
		my_msg += random_token();
	}

	return my_msg;
}



