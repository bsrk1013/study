#pragma once
#include <iostream>
#include "DBBD/TimerManager.h"
#include "SessionServer.h"

int main() {
	SessionServer server("Session", "127.0.0.1", 8101, 1);
	DBBD::TimerManager::Instance()->init(8);

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