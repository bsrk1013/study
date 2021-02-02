#include "MysqlManager.h"

namespace DBBD
{
	//using namespace mysqlx;

	std::string MysqlManager::getConn()
	{
		try {
			/*sql::Driver* driver*/
			//Session session("mysqlx://root@118.67.134.160");
		/*	Client client(
				SessionOption::USER, "root",
				SessionOption::PWD, "1231013a",
				SessionOption::HOST, "118.67.134.160",
				SessionOption::PORT, 3306,
				SessionOption::DB, "Test",
				SessionOption::SSL_MODE, SSLMode::DISABLED,
				ClientOption::POOLING, true,
				ClientOption::POOL_MAX_SIZE, 10,
				ClientOption::POOL_QUEUE_TIMEOUT, 1000,
				ClientOption::POOL_MAX_IDLE_TIME, 500
			);*/

			//std::string uri = "mysqlx://118.67.134.160:33060/Test?user=root&password=1231013a";
			//mysqlx::Session session("root:1231013a@118.67.134.160/Test");
			//auto db = session.getSchema("Test");
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}

		return "";
	}
}