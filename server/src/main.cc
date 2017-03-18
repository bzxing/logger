#include <iostream>

#include <string>
#include <vector>
#include <deque>

#include <cctype>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

typedef unsigned short PortType;


namespace Cfg
{
	//static const char * hostname = "localhost";
	static const PortType port = 9876;
	static const size_t session_buf_size = 4096;
	static const char * msg_delim = "\r\n";
};

class Session
	: public std::enable_shared_from_this<Session>
{
public:

	typedef std::string InMsg;
	typedef std::deque<InMsg> InQueue;


	Session(boost::asio::io_service & io_service)
	:	_socket(io_service),
		_in_buf(Cfg::session_buf_size)
	{
		std::cout << "Session constructed!\n";
	}

	~Session()
	{
		std::cout << "Session destroyed!\n";
	}

	void start()
	{
		std::cout << "Session started!\n";
		read();
	}

	tcp::socket & get_socket()
	{
		return _socket;
	}

private:
	void read()
	{
		std::shared_ptr<Session> self(shared_from_this());

		boost::asio::async_read_until(
			_socket,
			_in_buf,
			std::string(Cfg::msg_delim),
			[this, self]
				// N.B.: Must copying "self" by value to increase ref count
				// so that the calling object doesn't die before callback.
				(boost::system::error_code ec, std::size_t bytes_transferred)
			{
				// Async read callback function

				if (!ec)
				{
					std::cout << "Transferred: " << bytes_transferred << " Bytes\n";

					// Read buffer into temporary string
					//_in_buf.commit(bytes_transferred);
					std::istream is(&_in_buf);
					std::string in_msg;
					std::getline(is, in_msg);

					// Test raw message
					/*
					std::cout << "Read Raw: " << in_msg << std::endl;
					for (int c : in_msg)
					{
						std::cout << c << " ";
					}
					std::cout << std::endl;
					*/

					// Trim trailing whitespace
					while (!in_msg.empty())
					{
						char trailing_char = in_msg.back();
						if (!isprint(trailing_char) || isspace(trailing_char))
						{
							in_msg.pop_back();
						}
						else
						{
							break;
						}
					}
					// Delete all non-printables
					in_msg.erase(
						std::remove_if(
							in_msg.begin(),
							in_msg.end(),
							[](char c)
								{
									return !isprint(c);
								}
							),
						in_msg.end()
					);

					// If message still isn't empty, add it to the queue
					if (!in_msg.empty())
					{

						std::cout << "Read: " << in_msg << std::endl;
						for (int c : in_msg)
						{
							std::cout << c << " ";
						}
						std::cout << std::endl;

						_in_queue.push_back(std::move(in_msg));

						std::cout << "Updated queue Size: " << _in_queue.size() << std::endl;
					}

					read();
				}
			});
	}
/*
	void write(std::size_t length)
	{
		std::cout << "Sending: " << _data << std::endl;
		auto self(shared_from_this());
		boost::asio::async_write(
			_socket,
			boost::asio::buffer(_data, length),
			[this, self](boost::system::error_code ec, std::size_t)
			{
				if (!ec)
				{
					//read();
				}
			});
	}
*/
	tcp::socket _socket;

	boost::asio::streambuf	_in_buf;
	InQueue _in_queue;

	//static const size_t max_length = 1024;
	//char _data[max_length];
};

class Server
{
public:
	Server(
		boost::asio::io_service& io_service,
		PortType port)
	:
		_acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
		_io_service(io_service),
		_port(port)
	{

	}

	void run()
	{
		std::cout << "Listing on port...\n";
		std::shared_ptr<Session> session( new Session(_io_service) );

		_acceptor.async_accept(
			session->get_socket(),
			[this, session](boost::system::error_code ec)
				{
					std::cout << "accept ec: " << ec << std::endl;
					if (!ec)
					{
						session->start();
					}

					run();
				});
	}

private:

	tcp::acceptor _acceptor;
	//tcp::socket _socket;
	boost::asio::io_service & _io_service;
	const PortType _port;
};


int main(int argc, char * argv[])
{
	std::cout << argc << std::endl;

	for (int i = 0; i < argc; ++i)
	{
		std::cout << argv[i] << std::endl;
	}

	try
	{
		boost::asio::io_service io_service;

		Server s(io_service, Cfg::port);
		s.run();


		io_service.run();
	}
	catch (std::exception & e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	return 0;
}
