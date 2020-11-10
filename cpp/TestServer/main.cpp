#include <iostream>
#include <boost/asio.hpp>
#include "TestServer.hpp"

int main()
{
	boost::asio::io_context context;

	try {
		TestServer server(context);

		context.run();
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
}
