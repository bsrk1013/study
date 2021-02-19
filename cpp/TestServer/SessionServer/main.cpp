#pragma once
#include <iostream>
#include "DBBD/TimerManager.h"
#include "DBBD/RedisManager.h"
#include "DBBD/MariaDBManager.h"
#include "DBBD/Log.h"
#include "SessionServer.h"
#include "CommonConfig.h"
#include <locale>
#include <atlconv.h>

int main() {
	try {
		DBBD::Log::Instance()->init("Session");
		CommonConfig::Instance()->load();
		SessionServer server("Session", "127.0.0.1", 8101, 1);
		DBBD::TimerManager::Instance()->init(8);
		DBBD::RedisManager::Instance()->init(
			CommonConfig::Instance()->Redis.get<std::string>("ip"),
			CommonConfig::Instance()->Redis.get<int>("port"),
			CommonConfig::Instance()->Redis.get<std::string>("psw"));
		DBBD::MariaDBManager::Instance()->init(
			CommonConfig::Instance()->MariaDB.get<std::string>("ip"),
			CommonConfig::Instance()->MariaDB.get<int>("port"),
			CommonConfig::Instance()->MariaDB.get<std::string>("id"),
			CommonConfig::Instance()->MariaDB.get<std::string>("psw"),
			CommonConfig::Instance()->MariaDB.get<std::string>("db"));

		server.start();

		while (true) {
			std::string a;
			std::getline(std::cin, a);

			if (a == "exit") {
				server.stop();
				break;
			}
			else {
				LOG_ERROR(a);
			}
		}
	}
	catch (const std::exception& e) {
		LOG_ERROR(e.what());
	}

	return 0;
}