
#include <iostream>
#include <exception>

#include "frontend.hh"

namespace Cfg
{
	static const bool cli_arg_debug = false;
};

int main(int argc, char * argv[])
{
	std::cout << argc << std::endl;

	if (Cfg::cli_arg_debug)
	{
		for (int i = 0; i < argc; ++i)
		{
			std::cout << argv[i] << std::endl;
		}
	}

	try
	{
		start_server();
	}
	catch (std::exception & e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	return 0;
}
