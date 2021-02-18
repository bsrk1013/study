#pragma once
#include "DBBD/Singleton.h"
#include "DBBD/YamlParser.hpp"
#include "DBBD/Log.h"

class CommonConfig: public DBBD::Singleton<CommonConfig>
{
public:
	void load() {
		Common = DBBD::YamlParser::loadConfig("..\\..\\Common.yml");
		Redis = Common.get<DBBD::Config>("Redis");
		MariaDB = Common.get<DBBD::Config>("MariaDB");

		LOG_INFO("Config load complete...");
	}
	// Singleton을(를) 통해 상속됨
	virtual void release() override {
	}

public:
	DBBD::Config Common;
	DBBD::Config Redis;
	DBBD::Config MariaDB;
};