#pragma once
#include <iostream>
#include "CommunityServer.h"

int main() {
	CommunityServer server("Community", "127.0.0.1", 8100);
	
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