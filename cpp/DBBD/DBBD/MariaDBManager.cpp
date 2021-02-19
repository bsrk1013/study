#include "MariaDBManager.h"
#include "mysql.h"
#include "Log.h"

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
			putInfo(maria);
		}

		thread = NEW_THREAD_SP([this]() {
			while (true) {
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				update();
			}
			});

		isInit = true;
		LOG_INFO("{} init, ip: {}, port: {}", name, address, port);
	}

	void MariaDBManager::release()
	{
		update();

		for (auto info : infoSet) {
			closeInfoInternal(info);
		}

		infoSet.clear();
	}

	void MariaDBManager::update()
	{
		std::string totalQuery;

		std::scoped_lock lock(queueLockObject);
		if (queryQueue.empty()) { return; }

		bool loop = true;
		while (loop) {
			for (size_t i = 0; i < 1000; i++) {
				std::string query = queryQueue.front();
				queryQueue.pop();
				totalQuery += query += ";";

				if (queryQueue.empty()) {
					loop = false;
					break;
				}
			}

			execute(totalQuery);
		}
	}

	std::vector<std::map<std::string, std::string>> MariaDBManager::execute(std::string query)
	{
		std::vector<std::map<std::string, std::string>> result;

		auto maria = getInfo();
		int error = mysql_query(maria->conn, query.c_str());
		if (error) {
			std::string errorString = mysql_error(maria->conn);
			LOG_ERROR("MariaDBManager, exeQuery error({}), message: {}, query: {}", error, errorString, query);
			return result;
		}

		do {
			auto queryResult = mysql_store_result(maria->conn);
			if (queryResult) {
				int fieldCount = mysql_num_fields(queryResult);

				std::vector<std::string> fields;
				while (auto field = mysql_fetch_field(queryResult)) {
					fields.push_back(field->name);
				}

				while (auto row = mysql_fetch_row(queryResult)) {
					std::map<std::string, std::string> map;
					for (int i = 0; i < fieldCount; i++) {
						std::string field = fields[i];
						std::string value = row[i];

						map[field] = value;
					}
					result.push_back(map);
				}
				mysql_free_result(queryResult);
			}
			else {
				if (mysql_field_count(maria->conn) == 0) {
					mysql_affected_rows(maria->conn);
				}
				else {
					LOG_WARN("Could not retrieve result set");
					break;
				}
			}

			if ((error = mysql_next_result(maria->conn)) > 0) {
				LOG_WARN("Could not execute statement");
			}

		} while (!error);

		putInfo(maria);

		return result;
	}

	std::string MariaDBManager::queryBind(std::string origin, std::vector<std::any> args)
	{
		auto queryParts = strSplit(origin, '?');

		if (queryParts.size() != args.size()) {
			if (args.size() == 1 && queryParts.size() == 2) {}
			else {
				LOG_ERROR("illegal query bind, query: {}, argsCount: {}", origin, args.size());
				std::string error = "illegal query bind, query: " + origin + ", argsCount: " + std::to_string(args.size());
				throw std::exception(error.c_str());
			}
		}

		std::string query;
		for (size_t i = 0; i < queryParts.size(); i++) {
			query += queryParts[i];

			if (args.size() <= i) { continue; }

			query += "'";
			auto arg = args[i];
			if (auto castByte = arg._Cast<BYTE>()) {
				query += std::to_string(*castByte);
			}
			else if (auto castShort = arg._Cast<short>()) {
				query += std::to_string(*castShort);
			}
			else if (auto castInt = arg._Cast<int>()) {
				query += std::to_string(*castInt);
			}
			else if (auto castString = arg._Cast<std::string>()) {
				query += *castString;
			}
			else if (auto castChar = arg._Cast<const char*>()) {
				query += *castChar;
			}
			query += "'";
		}

		return query;
	}

	MariaSP MariaDBManager::createInfo()
	{
		MYSQL* mysql = mysql_init(NULL);
		mysql_real_connect(mysql, address.c_str(), user.c_str(), psw.c_str(), db.c_str(), port, NULL, 0);

		mysql_set_character_set(mysql, "euckr");
		mysql_options(mysql, MYSQL_INIT_COMMAND, "SET NAMES euckr");
		mysql_options(mysql, MARIADB_OPT_MULTI_STATEMENTS, (void*)"");
		mysql_options(mysql, MARIADB_OPT_MULTI_RESULTS, (void*)"");
		mysql_set_server_option(mysql, MYSQL_OPTION_MULTI_STATEMENTS_ON);

		MariaSP maria = std::make_shared<MariaConnInfo>();
		maria->conn = mysql;
		maria->usedTime = std::chrono::system_clock::now();

		return maria;
	}

	void MariaDBManager::closeInfoInternal(MariaSP info)
	{
		mysql_close(info->conn);
	}
}