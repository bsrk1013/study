#include "MysqlManager.h"

namespace DBBD
{
	std::string MysqlManager::getConn()
	{
		try {
			//std::string uri = "mysqlx://118.67.134.160:33060/Test?user=root&password=1231013a";
			mysqlx::Session session(
				mysqlx::SessionOption::HOST, "118.67.134.160",
				mysqlx::SessionOption::PORT, 33060,
				mysqlx::SessionOption::USER, "root",
				mysqlx::SessionOption::PWD, "1231013a");

			auto db = session.getSchema("Test");
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}

		return "";
	}
}