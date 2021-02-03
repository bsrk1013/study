#include "MariaDBManager.h"
//#include "mysql.h"

namespace DBBD
{
	void MariaDBManager::init(const std::string& address, const int& port,
		const std::string& user, const std::string& psw,
		const short& maxConnCount, const std::string& db)
	{
		this->address = address;
		this->port = port;
		this->user = user;
		this->psw = psw;
		this->maxConnCount = maxConnCount;
		this->db = db;

		isInit = true;
	}

	std::string MariaDBManager::getConn()
	{
		return "";
	}
}