#pragma once
#include <string>
#include <set>
#include <mutex>
#include "Define.h"
#include "Singleton.h"
#include "DBBaseManager.h"
#include "mysql.h"

namespace DBBD
{
	class MariaConnInfo
	{
	public:
		std::chrono::system_clock::time_point usedTime;
		MYSQL* conn;
	};

	using MariaSP = std::shared_ptr<MariaConnInfo>;
	using ExeParam = std::function<void(MYSQL_RES*)>;

	class MariaDBManager : public DBBaseManager<MariaSP>, public Singleton<MariaDBManager>
	{
	public:
		void init(const std::string& address, const int& port,
			const std::string& user, const std::string& psw,
			const std::string& db = "", const short& maxConnCount = 8);

	public:
		template <typename ... Args>
		void exeQuery(std::string query, Args ... args/*, ExeParam callback*/);
		void exeSP(std::string sp/*, ExeParam callback*/);

	private:
		void stmtBind(MYSQL_BIND* bind, std::vector<std::any> args);

	private:
		virtual MariaSP createInfo() override;
		virtual void closeInfoInternal(MariaSP info) override;

	private:
		std::string address;
		int port;
		std::string user;
		std::string psw;
		std::string db;
		short maxConnCount;
	};

	template <typename ... Args>
	void MariaDBManager::exeQuery(std::string query, Args ... args)
	{
		std::vector<std::any> argVec = { args... };
		int paramCount = argVec.size();

		auto maria = getInfo();
		MYSQL_STMT* stmt = mysql_stmt_init(maria->conn);
		MYSQL_BIND* bind = new MYSQL_BIND[paramCount];
		memset(bind, 0, sizeof(MYSQL_BIND));

		stmtBind(bind, argVec);

		std::string error;
		int errorCode = 0;
		my_bool result;
		/*if (mysql_stmt_prepare(stmt, query.c_str(), query.size())) {
			error = mysql_stmt_error(stmt);
			return;
		}*/

		result = mysql_stmt_attr_set(stmt, STMT_ATTR_PREBIND_PARAMS, &paramCount);
		result = mysql_stmt_bind_param(stmt, bind);
		errorCode = mariadb_stmt_execute_direct(stmt, query.c_str(), -1);

		auto storeResult = mysql_store_result(stmt->mysql);

		error = mysql_stmt_error(stmt);

		mysql_stmt_close(stmt);
		delete[] bind;
	}
}