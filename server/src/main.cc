#include <iostream>

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

typedef unsigned short PORT_TYPE;


namespace Cfg
{
	//static const char * hostname = "localhost";
	static const PORT_TYPE port = 9876;
};

class Session
	: public std::enable_shared_from_this<Session>
{
public:
	Session(boost::asio::io_service & io_service)
	:	_socket(io_service)
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
		do_read();
	}

	tcp::socket & get_socket()
	{
		return _socket;
	}

private:
	void do_read()
	{
		auto self(shared_from_this());
		_socket.async_read_some(
			boost::asio::buffer(_data, max_length),
			[this, self](boost::system::error_code ec, std::size_t length)
			{
				std::cout << "Read: " << _data << std::endl;
				if (!ec)
				{
					do_write(length);
				}
			});
	}

	void do_write(std::size_t length)
	{
		auto self(shared_from_this());
		boost::asio::async_write(
			_socket,
			boost::asio::buffer(_data, length),
			[this, self](boost::system::error_code ec, std::size_t)
			{
				if (!ec)
				{
					do_read();
				}
			});
	}

	tcp::socket _socket;

	static const size_t max_length = 1024;
	char _data[max_length];
};

class Server
{
public:
	Server(
		boost::asio::io_service& io_service,
		PORT_TYPE port)
	:
		_acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
		_io_service(io_service),
		_port(port)
	{
		std::cout << "Starting server...\n";
		do_accept();
	}

private:
	void do_accept()
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

					do_accept();
				});
	}

	tcp::acceptor _acceptor;
	//tcp::socket _socket;
	boost::asio::io_service & _io_service;
	const PORT_TYPE _port;
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

		io_service.run();
	}
	catch (std::exception & e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	return 0;
}
