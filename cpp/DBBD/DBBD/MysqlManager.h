#pragma once
#include <iostream>
#include "Singleton.h"
#include "mysqlx/xdevapi.h"

namespace DBBD
{
	class MysqlManager : public Singleton<MysqlManager>
	{
	public:
		std::string getConn();
	};
}