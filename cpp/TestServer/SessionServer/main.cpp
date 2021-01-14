#pragma once
#include <iostream>
#include "SessionServer.h"

int main() {
	SessionServer server("Session", "127.0.0.1", 8101, 1);
	server.start();

	while (true) {
		std::string a;
		std::getline(std::cin, a);

		if (a == "exit") {
			server.stop();
			break;
		}
	}

	return 0;
}