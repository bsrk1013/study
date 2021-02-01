#include "MysqlManager.h"

namespace DBBD
{
	std::string MysqlManager::getConn()
	{
		try {
			mysqlx::Session session("mysqlx://root@118.67.134.160");
		}
		catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
		/*sql::Driver* driver;
		sql::Connection* conn;

		driver = get_driver_instance();
		conn = driver->connect("tcp://118.67.134.160:3306", "root", "1231013a");*/

		//delete conn;

		return "";
	}
}