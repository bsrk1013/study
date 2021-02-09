#pragma once
#include <iostream>
#include "DBBD/TimerManager.h"
#include "DBBD/MysqlManager.h"
#include "DBBD/RedisManager.h"
#include "DBBD/MariaDBManager.h"
#include "SessionServer.h"

int main() {
	SessionServer server("Session", "127.0.0.1", 8101, 1);
	DBBD::TimerManager::Instance()->init(8);
	DBBD::RedisManager::Instance()->init("118.67.134.160", 6379);
	DBBD::MariaDBManager::Instance()->init("118.67.134.160", 3306, "root", "1231013a");


	try {
		server.start();

		while (true) {
			std::string a;
			std::getline(std::cin, a);

			if (a == "exit") {
				server.stop();
				break;
			}
		}
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}