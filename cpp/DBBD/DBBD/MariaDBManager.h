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

		auto maria = getInfo();
		MYSQL_STMT* stmt = mysql_stmt_init(maria->conn);
		MYSQL_BIND bind[argVec.size()];
	}
}