#include "CServer.h"

int main()
{
	try {
		unsigned short port = 8080;
		net::io_context ioc;
		boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
		signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
			if (error) {
				return 0;
			}
			ioc.stop();
		});
		std::make_shared<CServer>(ioc, port)->run();
		ioc.run();
	}
	catch (const std::exception& ec) {
		std::cerr << "CServer Error : " << ec.what() << std::endl;
		return -1;
	}
	return 0;
}

