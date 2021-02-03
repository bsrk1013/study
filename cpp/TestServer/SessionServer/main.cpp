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

	server.start();

	auto mysqlConn = DBBD::MysqlManager::Instance()->getConn();
	auto mariaConn = DBBD::MariaDBManager::Instance()->getConn();

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