#pragma once
#include <string>
#include <set>
#include <mutex>
#include "Define.h"
#include "Singleton.h"
#include "mysql.h"

namespace DBBD
{
	class MariaConnInfo
	{
	public:
		std::chrono::system_clock::time_point usedTime;
		std::shared_ptr<MYSQL> conn;
	};

	using MariaSP = std::shared_ptr<MariaConnInfo>;

	struct MariaOrder
	{
		bool operator()(MariaSP r1, MariaSP r2) const {
			return r1->usedTime > r2->usedTime;
		}
	};

	class MariaDBManager : public Singleton<MariaDBManager>
	{
	public:
		void init(const std::string& address, const int& port,
			const std::string& user, const std::string& psw, 
			const std::string& db = "", const short& maxConnCount = 8);

	private:
		std::string getConn();
		MariaSP createMaria();

	private:
		bool isInit = false;
		std::string address;
		int port;
		std::string user;
		std::string psw;
		std::string db;
		short maxConnCount;
		std::set<MariaSP, MariaOrder> mariaSet;
		std::mutex lockObject;
	};
}