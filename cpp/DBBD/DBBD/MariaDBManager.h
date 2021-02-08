#pragma once
#include <string>
#include <set>
#include <map>
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
	class MariaDBManager : public DBBaseManager<MariaSP>, public Singleton<MariaDBManager>
	{
	public:
		void init(const std::string& address, const int& port,
			const std::string& user, const std::string& psw,
			const std::string& db = "", const short& maxConnCount = 8);

	public:
		template <typename ... Args>
		std::map<std::string, std::string> exeQuery(std::string query, Args ... args);
		template <typename ... Args>
		std::map<std::string, std::string> exeSP(std::string sp, Args ... args);;

	private:
		std::map<std::string, std::string> execute(std::string query);
		std::string queryBind(std::string origin, std::vector<std::any> args);
		std::vector<std::string> split(std::string input, char delimiter);

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
	std::map<std::string, std::string> MariaDBManager::exeQuery(std::string query, Args ... args)
	{
		std::vector<std::any> argVec = { args... };
		auto resultQuery = queryBind(query, argVec);

		return execute(resultQuery);
	}

	template <typename ... Args>
	std::map<std::string, std::string> MariaDBManager::exeSP(std::string spName, Args ... args)
	{
		std::vector<std::any> argVec = { args... };
		int paramCount = argVec.size();

		std::string query = "call " + spName + "(";
		for (size_t i = 0; i < paramCount; i++) {
			query += "?";
			if (i + 1 < paramCount) {
				query += ", ";
			}
		}
		query += ")";
		std::string resultQuery = queryBind(query, argVec);

		return execute(resultQuery);
	}
}