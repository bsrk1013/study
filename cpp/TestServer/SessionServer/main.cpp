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
	DBBD::MariaDBManager::Instance()->init("118.67.134.160", 3306, "root", "1231013a", "Test");


	try {
		server.start();

		bool loop = true;
		std::string value = "THREAD_TEST";
		std::vector<DBBD::ThreadSP> threadList;
		for (size_t i = 0; i < 8; i++) {
			DBBD::ThreadSP thread = NEW_THREAD_SP([&]() {
				for (size_t j = 0; j < 1000; j++) {
					if (!loop) { break; }
					if ((j % 2) == 0) {
						DBBD::MariaDBManager::Instance()->enqueSP("TEST_INSERT_SP", value);
					}
					else {
						DBBD::MariaDBManager::Instance()->exeSP("TEST_INSERT_SP", value);
					}
				}
				});
			threadList.push_back(thread);
		}

		while (true) {
			std::string a;
			std::getline(std::cin, a);

			if (a == "exit") {
				loop = false;
				/*for (auto thread : threadList) {
					thread->join();
				}*/
				threadList.clear();

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