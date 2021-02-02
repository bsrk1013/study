#pragma once
#include <iostream>
#include "Singleton.h"

namespace DBBD
{
	class MysqlManager : public Singleton<MysqlManager>
	{
	public:
		std::string getConn();
	};
}