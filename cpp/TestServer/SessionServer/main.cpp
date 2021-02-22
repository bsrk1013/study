#pragma once
#include <iostream>
#include "DBBD/TimerManager.h"
#include "DBBD/RedisManager.h"
#include "DBBD/MariaDBManager.h"
#include "DBBD/Log.h"
//#include "DBBD/ExceptionHandler.h"
#include "SessionServer.h"
#include "CommonConfig.h"


int main() {
	try {
		//DBBD::ExceptionHandler::Instance()->init("Session");
		CommonConfig::Instance()->load();
		DBBD::Log::Instance()->init("Session")
			->usingTelegramBot(
				TELEGRAMBOT_CONFIG.get<std::string>("token"),
				TELEGRAMBOT_CONFIG.get<int>("chatid"));
		SessionServer server("Session", "127.0.0.1", 8101, 1);
		DBBD::TimerManager::Instance()->init(8);
		DBBD::RedisManager::Instance()->init(
			REDIS_CONFIG.get<std::string>("ip"),
			REDIS_CONFIG.get<int>("port"),
			REDIS_CONFIG.get<std::string>("psw"));
		DBBD::MariaDBManager::Instance()->init(
			MARIADB_CONFIG.get<std::string>("ip"),
			MARIADB_CONFIG.get<int>("port"),
			MARIADB_CONFIG.get<std::string>("id"),
			MARIADB_CONFIG.get<std::string>("psw"),
			MARIADB_CONFIG.get<std::string>("db"));

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