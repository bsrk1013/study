#include "MariaDBManager.h"
#include "mysql.h"

namespace DBBD
{
	void MariaDBManager::init(const std::string& address, const int& port,
		const std::string& user, const std::string& psw,
		const std::string& db, const short& maxConnCount)
	{
		this->address = address;
		this->port = port;
		this->user = user;
		this->psw = psw;
		this->db = db;
		this->maxConnCount = maxConnCount;

		for (size_t i = 0; i < maxConnCount; i++) {
			auto maria = createMaria();
		}

		isInit = true;
	}

	std::string MariaDBManager::getConn()
	{
		MYSQL* mysql = mysql_init(NULL);
		mysql_real_connect(mysql, address.c_str(), user.c_str(), psw.c_str(), db.c_str(), port, NULL, true);
		return "";
	}

	MariaSP MariaDBManager::createMaria()
	{
		std::shared_ptr<MYSQL> conn;
		MYSQL* mysql = mysql_init(NULL);
		mysql_real_connect(mysql, address.c_str(), user.c_str(), psw.c_str(), db.c_str(), port, NULL, 0);
		conn.reset(mysql);

		MariaSP maria = std::make_shared<MariaConnInfo>();
		maria->conn = conn;
		maria->usedTime = std::chrono::system_clock::now();
		return maria;
	}
}