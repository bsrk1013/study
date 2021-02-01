#pragma once
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
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