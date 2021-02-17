#pragma once
#include <iostream>
#include "DBBD/TimerManager.h"
#include "DBBD/RedisManager.h"
#include "DBBD/MariaDBManager.h"
#include "DBBD/json.hpp"
#include "DBBD/Common.hpp"
#include "SessionServer.h"
#include <locale>
#include <atlconv.h>

int main() {
	SessionServer server("Session", "127.0.0.1", 8101, 1);
	DBBD::TimerManager::Instance()->init(8);
	DBBD::RedisManager::Instance()->init("118.67.134.160", 6379);
	DBBD::MariaDBManager::Instance()->init("118.67.134.160", 3306, "root", "1231013a", "Test");

	try {
		nlohmann::json j1;
		j1["uid"] = 1;
		j1["nickname"] = DBBD::strconv("µµºñ");
		j1["level"] = 20;
		j1["ranking"] = 2400;

		std::string dump = j1.dump(-1, ' ', false, nlohmann::json::error_handler_t::replace);
		nlohmann::json j2 = nlohmann::json::parse(dump);

		std::string nickname = DBBD::strconv(j2["nickname"].get<std::wstring>());

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