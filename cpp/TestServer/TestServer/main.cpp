#include <boost/asio.hpp>
#include <iostream>
#include "SessionServer.h"

int main() {
	try {
		SessionServer server("Session");

		while (true) {
			std::string a;
			std::getline(std::cin, a);

			if (a == "exit") {
				break;
			}
		}
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}