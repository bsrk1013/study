#include "MariaDBManager.h"
#include "mysql.h"

namespace DBBD
{
	void MariaDBManager::init(const std::string& address, const int& port,
		const std::string& user, const std::string& psw,
		const std::string& db, const short& maxConnCount)
	{
		this->name = "MariaDBManager";
		this->address = address;
		this->port = port;
		this->user = user;
		this->psw = psw;
		this->db = db;
		this->maxConnCount = maxConnCount;

		for (size_t i = 0; i < maxConnCount; i++) {
			auto maria = createInfo();
			infoSet.insert(maria);
		}

		isInit = true;
		std::cout << name << " init, ip: " << address << ", port: " << port << std::endl;
	}

	void MariaDBManager::stmtBind(MYSQL_BIND* bind, std::vector<std::any> args)
	{
		for (size_t i = 0; i < args.size(); i++) {
			std::any arg = args[i];

			if (auto castByte = arg._Cast<BYTE>()) {
				bind[i].buffer_type = MYSQL_TYPE_TINY;
				bind[i].buffer = castByte;
			}
			else if (auto castShort = arg._Cast<short>()) {
				bind[i].buffer_type = MYSQL_TYPE_SHORT;
				bind[i].buffer = castShort;
			}
			else if (auto castInt = arg._Cast<int>()) {
				bind[i].buffer_type = MYSQL_TYPE_LONG;
				bind[i].buffer = castInt;
			}
			else if (auto castString = arg._Cast<std::string>()) {
				bind[i].buffer_type = MYSQL_TYPE_STRING;
				bind[i].buffer = (char*)castString->c_str();
				bind[i].buffer_length = castString->size();
			}
			else {
			}
		}
	}

	//void MariaDBManager::exeQuery(std::string query)
	//{
	//	auto maria = getInfo();
	//	MYSQL_STMT* stmt = mysql_stmt_init(maria->conn);



	//	/*int error = mysql_query(maria->conn, query.c_str());
	//	if (error) {
	//		std::cout << "MariaDBManager, exeQuery error, message: " << mysql_error(maria->conn) << std::endl;
	//		return;
	//	}

	//	auto result = mysql_store_result(maria->conn);
	//	int fieldCount = mysql_num_fields(result);
	//	
	//	MYSQL_ROW row;
	//	while (row = mysql_fetch_row(result))
	//	{
	//		for (int i = 0; i < fieldCount; i++) {
	//		}
	//	}

	//	mysql_free_result(result);*/

	//	return;
	//}

	MariaSP MariaDBManager::createInfo()
	{
		MYSQL* mysql = mysql_init(NULL);
		mysql_real_connect(mysql, address.c_str(), user.c_str(), psw.c_str(), db.c_str(), port, NULL, 0);

		mysql_set_character_set(mysql, "euckr");
		mysql_options(mysql, MYSQL_INIT_COMMAND, "SET NAMES euckr");

		MariaSP maria = std::make_shared<MariaConnInfo>();
		maria->conn = mysql;
		maria->usedTime = std::chrono::system_clock::now();
		return maria;
	}

	void MariaDBManager::closeInfoInternal(MariaSP info)
	{
		mysql_close(info->conn);
		delete info->conn;
	}
}