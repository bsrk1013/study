#pragma once
#include <iostream>
#include "Singleton.h"

namespace DBBD
{
	class MariaDBManager : public Singleton<MariaDBManager>
	{
	public:
		void init(const std::string& address, const int& port,
			const std::string& user, const std::string& psw, 
			const short& maxConnCount = 8, const std::string& db = "");

	public:
		std::string getConn();

	private:
		bool isInit = false;
		std::string address;
		int port;
		std::string user;
		std::string psw;
		short maxConnCount;
		std::string db;
	};
}