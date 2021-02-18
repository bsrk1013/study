#pragma once
#include "DBBD/Cell.h"
#include "DBBD/Request.h"
#include "DBBD/Response.h"
#include "DBBD/Common.hpp"
#include "ProtocolType.hpp"

#include <map>

namespace ProtocolType {
	enum Value {
		PingCheckReq = 1001,
		PingCheckResp = 1002,
	};
	std::map<Value, std::string> stringMap = {
		{ Value::PingCheckReq, "PingCheckReq" },
		{ Value::PingCheckResp, "PingCheckResp" },
	};
	std::string Get(Value value) {
		auto iter = ProtocolType::stringMap.find(value);
		if (iter == ProtocolType::stringMap.end()) {
			return "";
		}
		return iter->second;
	}
}
