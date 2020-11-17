#include <boost/asio.hpp>
#include "SessionServer.h"

int main() {
	try {
		SessionServer server("Session");
	}
	catch (std::exception& e) {

	}

	return 0;
}